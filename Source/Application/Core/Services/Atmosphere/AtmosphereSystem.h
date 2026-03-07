#pragma once

#include <Application/Core/Core.h>
#include <Application/Core/Services/Managers/ResourceManager/ResourceManager.h>
#include <Application/Resource/Camera/Camera.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Material/Material.h>

namespace Nyx
{
	class AtmosphereSystem : public Singleton<AtmosphereSystem>
	{
	public:
		void Initialize()
		{
			m_shader = ResourceManager::GetShader(
				"AtmosphereShader",
				R"(Nyx\Source\Application\Shaders\Atmosphere\atmosphere.vert)",
				R"(Nyx\Source\Application\Shaders\Atmosphere\atmosphere.frag)"
			);
		}

		void DrawAtmosphere(const Camera& camera, const Transform& cameraTransform)
		{
			m_shader.Use();

			const uint32 shaderID = m_shader.GetID();

			LightingSystem::Get().UploadToShader(shaderID);

			const Math::Mat4f view = camera.GetViewMatrix();
			const Math::Mat4f proj = camera.GetProjectionMatrix();

			glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
            glUniform3f(glGetUniformLocation(shaderID, "uCameraPos"), 0.0f, 0.0f, 0.0f);

            for (EntityID id : ECS::Get().View<Transform, Sphere, AtmosphereComponent>())
            {
                const Transform& t = *ECS::Get().GetComponent<Transform>(id);
                const AtmosphereComponent& atmo = *ECS::Get().GetComponent<AtmosphereComponent>(id);

                Sphere& sphere = *ECS::Get().GetComponent<Sphere>(id);

                auto pos = t.position / METERS_PER_UNIT;
                auto sca = t.scale / METERS_PER_UNIT;
                auto rot = t.rotation;

                const Position& cameraPos = cameraTransform.position;
                Math::Vec3d relPos = Math::Vec3d(pos.GetWorld() - cameraPos.GetWorld());
                Math::Mat4f model = glm::translate(Math::Mat4d(1.0), relPos) * rot.ToMatrix() * sca.ToMatrix();

                model = glm::scale(model, glm::vec3(1.0f + atmo.thickness));
                glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

                // Atmosphere params
                glUniform3fv(glGetUniformLocation(shaderID, "uAtmoColor"), 1, glm::value_ptr(atmo.color));
                glUniform1f(glGetUniformLocation(shaderID, "uAtmoIntensity"), atmo.intensity);
                glUniform1f(glGetUniformLocation(shaderID, "uRimPower"), atmo.rimPower);
                glUniform1f(glGetUniformLocation(shaderID, "uLightPower"), atmo.lightPower);

				ImmediatePipeline::Get().Begin();
				ImmediatePipeline::Get().UseAtmosphere();
				glDrawElements(GL_TRIANGLES, sphere.m_sphereMesh.ebo.m_indexCount, GL_UNSIGNED_INT, 0);
				ImmediatePipeline::Get().End();
            }
		}

	private:
		Shader m_shader;
	};
}