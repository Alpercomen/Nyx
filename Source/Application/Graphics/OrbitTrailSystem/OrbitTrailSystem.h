#pragma once
#include <Application/Core/Core.h>
#include <Application/Services/Managers/ResourceManager/ResourceManager.h>
#include <Application/Resource/Components/Camera/Camera.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Components/Material/Material.h>

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

		void Update()
        {
            for (EntityID entityId : ECS::Get().View<Transform, OrbitTrail, Rigidbody>())
            {
                Transform& transform = *ECS::Get().GetComponent<Transform>(entityId);
                OrbitTrail& trail = *ECS::Get().GetComponent<OrbitTrail>(entityId);
                Rigidbody& rigidbody = *ECS::Get().GetComponent<Rigidbody>(entityId);

                if (!trail.enabled)
                    continue;

                Position position = transform.position;

                if (rigidbody.orbitalParentID != NO_ID && ECS::Get().HasComponent<Transform>(rigidbody.orbitalParentID))
                {
                    Transform& parentTransform = *ECS::Get().GetComponent<Transform>(rigidbody.orbitalParentID);
                    Position parentPosition = parentTransform.position;

                    position.SetWorld(position.GetWorld() - parentPosition.GetWorld());
                }

                // After all the precise calculations, reduce it for rendering.
                position.SetWorld(position.GetWorld() / METERS_PER_UNIT);

                if (trail.points.empty())
                {
                    trail.points.push_back({ position, 0.0 });
                    continue;
                }

                const auto& newest = trail.points.front();

                float64 distance = glm::length(position.GetWorld() - newest.position.GetWorld());

                if (distance < trail.sampleDistance)
                    continue;

                trail.points.push_front({ position, 0.0 });

                // Update distances.
                float64 accumulated = 0.0;

                for (size_t i = 1; i < trail.points.size(); ++i)
                {
                    accumulated += glm::length(trail.points[i - 1].position.GetWorld() - trail.points[i].position.GetWorld());

                    trail.points[i].distance = accumulated;
                }

                // Remove old points.
                while (!trail.points.empty() && trail.points.back().distance > trail.maxLength)
                    trail.points.pop_back();

                while (trail.points.size() > trail.maxPoints)
                    trail.points.pop_back();
            }
        }

		void DrawTrail(const Camera& camera, const Transform& cameraTransform)
        {
            m_vertices.clear();
            m_shader.Use();

            const uint32 shaderID = m_shader.GetID();

            const Math::Mat4f view = camera.GetViewMatrix();
            const Math::Mat4f proj = camera.GetProjectionMatrix();

            for (EntityID entityId : ECS::Get().View<Transform, OrbitTrail, Rigidbody>())
            {
                Transform& transform = *ECS::Get().GetComponent<Transform>(entityId);
                OrbitTrail& trail = *ECS::Get().GetComponent<OrbitTrail>(entityId);
                Rigidbody& rigidbody = *ECS::Get().GetComponent<Rigidbody>(entityId);

                if (!trail.enabled)
                    continue;

                if (trail.points.size() < 2)
                    continue;

                for (size_t i = 0; i + 1 < trail.points.size(); ++i)
                {
                    auto p0 = trail.points[i];
                    auto p1 = trail.points[i + 1];

                    if (rigidbody.orbitalParentID != NO_ID && ECS::Get().HasComponent<Transform>(rigidbody.orbitalParentID))
                    {
                        Transform& parentTransform = *ECS::Get().GetComponent<Transform>(rigidbody.orbitalParentID);
                        Position parentPosition = parentTransform.position;

                        parentPosition.SetWorld(parentPosition.GetWorld() / METERS_PER_UNIT);

                        p0.position.SetWorld(p0.position.GetWorld() + parentPosition.GetWorld());
                        p1.position.SetWorld(p1.position.GetWorld() + parentPosition.GetWorld());
                    }

                    Math::Vec3d relative0 = p0.position.GetWorld() - cameraTransform.position.GetWorld();
                    Math::Vec3d relative1 = p1.position.GetWorld() - cameraTransform.position.GetWorld();

                    float alpha0 = std::pow(1.0f - static_cast<float>(p0.distance / trail.maxLength), trail.fadePower);
                    float alpha1 = std::pow(1.0f - static_cast<float>(p1.distance / trail.maxLength), trail.fadePower);

                    m_vertices.push_back(TrailVertex{ Math::Vec3f(relative0), trail.color, alpha0 * trail.opacity });
                    m_vertices.push_back(TrailVertex{ Math::Vec3f(relative1), trail.color, alpha1 * trail.opacity });
                }

                if (m_vertices.empty())
                    return;

                glBindBuffer(GL_ARRAY_BUFFER, m_vbo.m_data);
                glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(TrailVertex), m_vertices.data(), GL_DYNAMIC_DRAW);

                glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));

                glBindVertexArray(m_vao.m_data);

                ImmediatePipeline::Get().Begin();
                ImmediatePipeline::Get().UseTrail();
                glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertices.size()));
                ImmediatePipeline::Get().End();

                glBindVertexArray(0);
            }
        }

	private:
		Shader m_shader;

		VAO m_vao;
		VBO m_vbo;

		Vector<TrailVertex> m_vertices;
	};
}