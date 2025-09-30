#pragma once
#include "Window.h"

#include <Application/Core/Core.h>
#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Core/Services/CameraService/CameraService.h>

namespace Nyx
{
    static InputEvent event;

    bool firstMouse = true;
    float lastX = 400.0f;
    float lastY = 400.f;

    void InputHelper::ProcessMouseMovement()
    {

        InputEventDispatcher::Get().AddCallback(EventType::MOUSE_MOVE, [&](const InputEvent& event)
        {
            auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

            if (cameraIDs.size() <= 0)
                return;

            const EntityID& id = cameraIDs[0];

            auto& camera = *ECS::Get().GetComponent<Camera>(id);

            double xPos = event.m_eventList.mouseX;
            double yPos = event.m_eventList.mouseY;

            if (firstMouse)
            {
                lastX = (float)xPos;
                lastY = (float)yPos;
                firstMouse = false;
            }

            float xoffset = (float)xPos - lastX;
            float yoffset = lastY - (float)yPos; // reversed y

            if (CameraService().Get().enabled) 
            {
                if (firstMouse)
                {
                    lastX = (float)xPos;
                    lastY = (float)yPos;
                    firstMouse = false;
                }

                float xoffset = (float)xPos - lastX;
                float yoffset = lastY - (float)yPos;

                lastX = (float)xPos;
                lastY = (float)yPos;

                CameraService().Get().yaw += xoffset * 0.1f;
                CameraService().Get().pitch += yoffset * 0.1f;
                CameraService().Get().pitch = glm::clamp(CameraService().Get().pitch, -89.0f, 89.0f);
                return;
            }

            lastX = (float)xPos;
            lastY = (float)yPos;

            camera.ProcessMouseMovement(xoffset, yoffset);
        });

    }

    void InputHelper::ProcessMouseScroll()
    {
        InputEventDispatcher::Get().AddCallback(EventType::MOUSE_SCROLL_WHEEL, [&](const InputEvent& event)
        {
            auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

            if (cameraIDs.size() <= 0)
                return;

            const EntityID& id = cameraIDs[0];

            auto& camera = *ECS::Get().GetComponent<Camera>(id);

            float scroll = event.m_eventList.scrollDelta;

            if (CameraService().Get().enabled) {
                CameraService().Get().distance *= (1.0f - scroll * 0.1f);

                const float& minimumDistance = CameraService().Get().minimumDistance;
                CameraService().Get().distance = glm::clamp(CameraService().Get().distance, minimumDistance, minimumDistance * 1000);
                return;
            }

            float currentSpeed = camera.GetMovementSpeed();
            float step = currentSpeed * 0.1f;
            float newSpeed = currentSpeed + scroll * step;

            newSpeed = glm::clamp(newSpeed, 0.001f, 1000.0f);
            camera.SetMovementSpeed(newSpeed);

            spdlog::info("Camera speed updated: {:.3f}", newSpeed);
        });
    }
}

