#pragma once
#include <algorithm>
#include <cmath>

#include <Application/Core/Core.h>
#include <Application/Services/Managers/ResourceManager/ResourceManager.h>
#include <Application/Resource/Components/Camera/Camera.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Components/Material/Material.h>
#include <Application/Core/Physics/Types.h>

namespace Nyx
{
	struct TrailVertex
	{
		Math::Vec3f position;
		Math::Vec3f color;
		float32 alpha;
	};

	class OrbitTrailSystem : public Singleton<OrbitTrailSystem>
	{
	public:
		void Initialize()
		{
			m_shader = ResourceManager::GetShader(
				"OrbitTrail",
				R"(Nyx\Source\Application\Shaders\Trail\trail.vert)",
				R"(Nyx\Source\Application\Shaders\Trail\trail.frag)"
			);

			glGenVertexArrays(1, &m_vao.m_data);
			glGenBuffers(1, &m_vbo.m_data);

			glBindVertexArray(m_vao.m_data);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo.m_data);
			glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

			// Position
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), reinterpret_cast<void*>(offsetof(TrailVertex, position)));

			// Color
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), reinterpret_cast<void*>(offsetof(TrailVertex, color)));

			// Alpha
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), reinterpret_cast<void*>(offsetof(TrailVertex, alpha)));

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		// Derives each body's osculating (instantaneous Keplerian) orbit around its orbital parent
		// from its current state vectors.
		void Update()
		{
			auto& ecs = ECS::Get();

			for (EntityID entityId : ecs.View<Transform, OrbitTrail, Rigidbody>())
			{
				Transform& transform = *ecs.GetComponent<Transform>(entityId);
				OrbitTrail& trail = *ecs.GetComponent<OrbitTrail>(entityId);
				Rigidbody& rigidbody = *ecs.GetComponent<Rigidbody>(entityId);

				OrbitalElements& orbit = trail.orbit;
				orbit.valid = false;

				if (!trail.enabled)
					continue;

				const EntityID parentID = rigidbody.orbitalParentID;

				if (parentID == NO_ID || parentID == entityId ||
					!ecs.HasComponent<Transform>(parentID) || !ecs.HasComponent<Rigidbody>(parentID))
					continue;

				const Transform& parentTransform = *ecs.GetComponent<Transform>(parentID);
				const Rigidbody& parentBody = *ecs.GetComponent<Rigidbody>(parentID);

				const float64 mu = G * parentBody.mass;

				if (mu <= 0.0)
					continue;

				// State relative to the parent (meters, meters/second).
				const Math::Vec3d r = transform.position.GetWorld() - parentTransform.position.GetWorld();
				const Math::Vec3d v = rigidbody.velocity.GetWorld() - parentBody.velocity.GetWorld();

				const float64 rLen = glm::length(r);

				if (rLen < 1e-6)
					continue;

				// Specific angular momentum. Zero means a purely radial path, which has no ellipse.
				const Math::Vec3d h = glm::cross(r, v);
				const float64 hLen = glm::length(h);

				if (hLen < 1e-9)
					continue;

				// Eccentricity vector: points from the parent to periapsis.
				const Math::Vec3d eVec = glm::cross(v, h) / mu - r / rLen;
				const float64 e = glm::length(eVec);

				// Specific orbital energy. >= 0 means unbound (parabolic/hyperbolic), which is not drawn.
				const float64 energy = 0.5 * glm::dot(v, v) - mu / rLen;

				if (energy >= 0.0 || e >= 1.0)
					continue;

				const float64 a = -mu / (2.0 * energy);
				const float64 b = a * std::sqrt(1.0 - e * e);

				// Perifocal basis. For (near) circular orbits periapsis is undefined, so use the current position.
				const Math::Vec3d W = h / hLen;
				const Math::Vec3d P = (e > 1e-9) ? (eVec / e) : (r / rLen);
				const Math::Vec3d Q = glm::cross(W, P);

				// Where the body currently sits on the ellipse (eccentric anomaly).
				const float64 x0 = glm::dot(r, P);
				const float64 y0 = glm::dot(r, Q);
				const float64 E0 = std::atan2(y0 / b, x0 / a + e);

				orbit.valid = true;
				orbit.semiMajorAxis = a;
				orbit.semiMinorAxis = b;
				orbit.eccentricity = e;
				orbit.trueAnomaly = E0;
				orbit.P = P;
				orbit.Q = Q;
			}
		}

		// Draws every body's orbit as a line strip that starts at the body and runs backwards
		// along its path, fading out with distance. Visually this reads as a trail of visited positions.
		void DrawTrail(const Camera& camera, const Transform& cameraTransform)
		{
			auto& ecs = ECS::Get();

			m_vertices.clear();

			const Math::Vec3d cameraPosition = cameraTransform.position.GetWorld();

			for (EntityID entityId : ecs.View<Transform, OrbitTrail, Rigidbody>())
			{
				const OrbitTrail& trail = *ecs.GetComponent<OrbitTrail>(entityId);
				const OrbitalElements& orbit = trail.orbit;
				const Rigidbody& rigidbody = *ecs.GetComponent<Rigidbody>(entityId);

				if (!trail.enabled || !orbit.valid)
					continue;

				const Transform& parentTransform = *ecs.GetComponent<Transform>(rigidbody.orbitalParentID);

				// Parent position in render units, already camera-relative (double precision).
				const Math::Vec3d parentOrigin = parentTransform.position.GetWorld() / METERS_PER_UNIT - cameraPosition;

				const uint32 segments = std::max<uint32>(trail.segments, 2u);
				const float64 sweep = 2.0 * glm::pi<float64>() * glm::clamp(static_cast<float64>(trail.trailFraction), 0.0, 1.0);

				// t = 0 is the body, t = 1 is the far end of the visible arc.
				// Going backwards along the path means decreasing eccentric anomaly.
				auto SamplePoint = [&](float64 t) -> Math::Vec3f
					{
						const float64 E = orbit.trueAnomaly - sweep * t;

						const float64 x = orbit.semiMajorAxis * (std::cos(E) - orbit.eccentricity);
						const float64 y = orbit.semiMinorAxis * std::sin(E);

						const Math::Vec3d local = (orbit.P * x + orbit.Q * y) / METERS_PER_UNIT;

						// Only cast to float after everything large has cancelled out.
						return Math::Vec3f(parentOrigin + local);
					};

				auto SampleAlpha = [&](float64 t) -> float32
					{
						const float32 fade = std::pow(1.0f - static_cast<float32>(t), trail.fadePower);
						return (trail.minAlpha + (1.0f - trail.minAlpha) * fade) * trail.opacity;
					};

				Math::Vec3f prevPosition = SamplePoint(0.0);
				float32 prevAlpha = SampleAlpha(0.0);

				for (uint32 i = 1; i <= segments; ++i)
				{
					const float64 t = static_cast<float64>(i) / static_cast<float64>(segments);

					const Math::Vec3f position = SamplePoint(t);
					const float32 alpha = SampleAlpha(t);

					m_vertices.push_back(TrailVertex{ prevPosition, trail.color, prevAlpha });
					m_vertices.push_back(TrailVertex{ position, trail.color, alpha });

					prevPosition = position;
					prevAlpha = alpha;
				}
			}

			if (m_vertices.empty())
				return;

			m_shader.Use();

			const uint32 shaderID = m_shader.GetID();

			const Math::Mat4f view = camera.GetViewMatrix();
			const Math::Mat4f proj = camera.GetProjectionMatrix();

			glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));

			glBindVertexArray(m_vao.m_data);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo.m_data);
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(TrailVertex), m_vertices.data(), GL_DYNAMIC_DRAW);

			ImmediatePipeline::Get().Begin();
			ImmediatePipeline::Get().UseTrail();
			glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertices.size()));
			ImmediatePipeline::Get().End();

			glBindVertexArray(0);
		}

	private:
		Shader m_shader;

		VAO m_vao;
		VBO m_vbo;

		Vector<TrailVertex> m_vertices;
	};
}
