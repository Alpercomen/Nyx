#pragma once
#include "Camera.h"

#include <Application/Core/Core.h>
#include <Application/Window/Window.h>
#include <Application/Constants/Constants.h>
#include <Application/Core/Services/Input/InputDispatcher.h>
#include <Application/Core/Services/Input/InputEvent.h>
#include <Application/Core/Services/Input/InputQueue.h>
#include <Application/Core/Services/CameraService/CameraService.h>
#include <Application/Resource/Components/Components.h>

Camera::Camera()
{
    SetFront(Math::Vec3f(0.0f, 0.0f, -1.0f));
    SetMovementSpeed(3);
    SetMovementSpeedMultiplier(5.f);
    SetMouseSensitivity(0.1f);
    SetZoom(45.0f);
    SetYaw(-90.0f);
    SetPitch(0.0f);
    SetWorldUp(Math::Vec3f(0.0f, 1.0f, 0.0f));

    UpdateCameraVectors();

    InputHelper::ProcessMouseMovement();
    InputHelper::ProcessMouseScroll();
}

glm::mat4 Camera::GetViewMatrix() 
{
    auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

    if (cameraIDs.size() <= 0)
        return Math::Mat4d(0.0);

    // Called every frame inside your render/update loop if follow is enabled
    if (CameraService().Get().enabled)
    {
        const EntityID& targetID = CameraService().Get().targetEntity;

        if (ECS::Get().HasComponent<Transform>(targetID))
            FocusCamera(targetID);
    }

    return glm::lookAt(glm::vec3(0.0f), GetFront(), GetUp());
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(GetZoom()), GetAspectRatio(), GetNearPlane(), GetFarPlane());
}

void Camera::FocusCamera(const EntityID& targetID)
{
    const auto& targetTransform = *ECS::Get().GetComponent<Transform>(targetID);
    const Position& pos = targetTransform.position / METERS_PER_UNIT;
    const Math::Vec3f& targetPos = pos.GetWorld();

    float distance = CameraService().Get().distance;
    float yaw = CameraService().Get().yaw;
    float pitch = CameraService().Get().pitch;

    // Spherical to Cartesian
    Math::Vec3f direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(direction);

    Math::Vec3f cameraPos = targetPos - direction * distance;

    // Update camera's Transform
    GetPosition().SetWorld(cameraPos);

    // Update camera direction
    SetFront(glm::normalize(targetPos - cameraPos));
    SetRight(glm::normalize(glm::cross(GetFront(), GetWorldUp())));
    SetUp(glm::cross(GetRight(), GetFront()));
}

void Camera::ProcessKeyboardMovement(Camera_Movement direction, float deltaTime)
{
    float velocity = GetMovementSpeed() * deltaTime;
    auto& pos = this->m_worldPos;

    switch (direction) {
        case FORWARD:
            spdlog::info("Move forward input detected by {:03.6f} unit.", velocity);
            pos.SetWorld(pos.GetWorld() + GetFront() * velocity);
            break;
        case BACKWARD:
            spdlog::info("Move backward input detected by {:03.6f} unit.", velocity);
            pos.SetWorld(pos.GetWorld() - GetFront() * velocity);
            break;
        case RIGHT:
            spdlog::info("Move right input detected by {:03.6f} unit.", velocity);
            pos.SetWorld(pos.GetWorld() + GetRight() * velocity);
            break;
        case LEFT:
            spdlog::info("Move left input detected by {:03.6f} unit.", velocity);
            pos.SetWorld(pos.GetWorld() - GetRight() * velocity);
            break;
        case UP:
            spdlog::info("Move up input detected by {:03.6f} unit.", velocity);
            pos.SetWorld(pos.GetWorld() + GetWorldUp() * velocity);
            break;
        case DOWN:
            spdlog::info("Move down input detected by {:03.6f} unit.", velocity);
            pos.SetWorld(pos.GetWorld() - GetWorldUp() * velocity);
            break;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= GetMouseSensitivity();
    yoffset *= GetMouseSensitivity();

    SetYaw(GetYaw() + xoffset);
    SetPitch(GetPitch() + yoffset);

    if (constrainPitch)
    {
        if (GetPitch() > 89.0f)
            SetPitch(89.0f);
        if (GetPitch() < -89.0f)
            SetPitch(-89.0f);
    }

    spdlog::info("Camera movement = x:{:3.6f}, y:{:3.6f}", xoffset, yoffset);

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    Math::Vec3f front;
    front.x = cos(glm::radians(GetYaw())) * cos(glm::radians(GetPitch()));
    front.y = sin(glm::radians(GetPitch()));
    front.z = sin(glm::radians(GetYaw())) * cos(glm::radians(GetPitch()));
    SetFront(glm::normalize(front));

    SetRight(glm::normalize(glm::cross(GetFront(), GetWorldUp())));
    SetUp(glm::normalize(glm::cross(GetRight(), GetFront())));
}
