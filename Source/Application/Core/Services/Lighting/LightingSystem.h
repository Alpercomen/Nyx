#pragma once
#include <GL/glew.h>

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Lighting/Light.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

namespace Nyx
{
	class LightingSystem : public Singleton<LightingSystem>
	{
	public:
		Vector<LightComponent*> directionalLights;
		Vector<LightComponent*> pointLights;

		void GatherLights()
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
					pointLights.push_back(light);
					break;
				}
			}
		}

		void UploadToShader(uint32 shaderID)
		{
			int count = std::min((int)directionalLights.size(), MAX_DIRECTIONAL_LIGHTS);
			glUniform1i(glGetUniformLocation(shaderID, "uDirLightCount"), count);

			for (int i = 0; i < count; ++i)
			{
				const auto* l = directionalLights[i];
				String base = "uDirectionalLights[" + std::to_string(i) + "]";

				glUniform3fv(glGetUniformLocation(shaderID, (base + ".direction").c_str()), 1, glm::value_ptr(l->direction));
				glUniform3fv(glGetUniformLocation(shaderID, (base + ".color").c_str()), 1, glm::value_ptr(l->color));
				glUniform1f(glGetUniformLocation(shaderID, (base + ".intensity").c_str()), l->intensity);
			}
		}
	};
}