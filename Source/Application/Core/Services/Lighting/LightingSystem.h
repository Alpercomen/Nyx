#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Lighting/Light.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Camera/Camera.h>

namespace Nyx
{
	class LightingSystem : public Singleton<LightingSystem>
	{
	public:
		Vector<LightComponent*> directionalLights;
		Vector<LightComponent*> pointLights;

		void GatherLights(Camera& camera)
		{
			directionalLights.clear();
			pointLights.clear();

			for (EntityID entityID : ECS::Get().View<LightComponent>())
			{
				auto* light = ECS::Get().GetComponent<LightComponent>(entityID);

				switch (light->type)
				{
				case LightType::DIRECTIONAL:
					directionalLights.push_back(light);
					break;
				case LightType::POINT:
				{
					if (!ECS::Get().HasComponent<Transform>(entityID))
						continue;

					const Transform& transform = *ECS::Get().GetComponent<Transform>(entityID);
					light->position = transform.position / METERS_PER_UNIT;
					light->position.SetWorld(light->position.GetWorld() - camera.GetPosition().GetWorld());

					pointLights.push_back(light);
					break;
				}
				}
			}
		}

		void UploadToShader(uint32 shaderID)
		{
			int count = std::min((int)directionalLights.size(), MAX_DIRECTIONAL_LIGHTS);
			GL::Get()->glUniform1i(GL::Get()->glGetUniformLocation(shaderID, "uDirLightCount"), count);

			for (int i = 0; i < count; ++i)
			{
				const auto l = directionalLights[i];
				String base = "uDirectionalLights[" + std::to_string(i) + "]";

				GL::Get()->glUniform3fv(GL::Get()->glGetUniformLocation(shaderID, (base + ".direction").c_str()), 1, glm::value_ptr(l->direction));
				GL::Get()->glUniform3fv(GL::Get()->glGetUniformLocation(shaderID, (base + ".color").c_str()), 1, glm::value_ptr(l->color));
				GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, (base + ".intensity").c_str()), l->intensity);
			}

			count = std::min((int)pointLights.size(), MAX_POINT_LIGHTS);
			GL::Get()->glUniform1i(GL::Get()->glGetUniformLocation(shaderID, "uPointLightCount"), count);

			for (int i = 0; i < count; ++i)
			{
				const auto l = pointLights[i];
				String base = "uPointLights[" + std::to_string(i) + "]";

				GL::Get()->glUniform3fv(GL::Get()->glGetUniformLocation(shaderID, (base + ".position").c_str()), 1, glm::value_ptr(l->position.GetWorld()));
				GL::Get()->glUniform3fv(GL::Get()->glGetUniformLocation(shaderID, (base + ".color").c_str()), 1, glm::value_ptr(l->color));
				GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, (base + ".intensity").c_str()), l->intensity);
				GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, (base + ".range").c_str()), l->range);
				GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, (base + ".decay").c_str()), l->decay);
			}
		}
	};
}