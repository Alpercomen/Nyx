#pragma once
#include <iostream>
#include <vector>

#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Camera/Camera.h>
#include <Application/Core/Physics/Meter.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Core/Services/Managers/SceneManager/SceneManager.h>
#include <Application/Resource/Material/ShaderProgram/ShaderProgram.h>
#include <Application/Core/Services/Lighting/LightingSystem.h>
#include <Application/Resource/Components/Components.h>


namespace Nyx
{
	class Renderer 
	{
	public:
        bool m_gridEnabled = true;

        Renderer()
        {
            
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
        }

    private:
        GridMesh m_grid;

	};
}