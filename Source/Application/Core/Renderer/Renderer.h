#pragma once
#include <iostream>
#include <vector>

#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Components/Camera/Camera.h>
#include <Application/Core/Physics/Meter.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Services/Managers/SceneManager/SceneManager.h>
#include <Application/Resource/Components/Material/Shader/Shader.h>
#include <Application/Services/Lighting/LightingSystem.h>
#include <Application/Services/Atmosphere/AtmosphereSystem.h>


namespace Nyx
{
	class Renderer 
	{
	public:
        bool m_gridEnabled = true;

        Renderer()
        {
            AtmosphereSystem::Get().Initialize();
        }

        void DrawScene(Scene& scene)
        {
            const Camera& camera = *ECS::Get().GetComponent<Camera>(scene.GetActiveCameraID());
            const Transform& transform = *ECS::Get().GetComponent<Transform>(scene.GetActiveCameraID());

            if (m_gridEnabled)
            {
                m_grid.DrawGrid(camera, transform);
            }

            LightingSystem::Get().GatherLights(transform);

            for (size_t i = 0; i < scene.GetSceneObjectSize(); ++i)
            {
                const auto& object = scene.GetSceneObject(i);
                object->Draw(camera, transform);
            }

            AtmosphereSystem::Get().DrawAtmosphere(camera, transform);
        }

    private:
        GridMesh m_grid;

	};
}