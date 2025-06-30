#include "Camera.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <Application/Core/Core.h>
#include <Application/Window/Window.h>
#include <Application/Constants/Constants.h>
#include <Application/Core/Input/InputDispatcher.h>
#include <Application/Core/Input/InputEvent.h>
#include <Application/Core/Input/InputQueue.h>
#include <Application/Resource/Components/Components.h>

Camera::Camera()
{
    SetFront(glm::vec3(0.0f, 0.0f, -1.0f));
    SetMovementSpeed(METERS_PER_UNIT * 10);
    SetMovementSpeedMultiplier(3.5f);
    SetMouseSensitivity(0.1f);
    SetZoom(45.0f);
    SetYaw(-90.0f);
    SetPitch(0.0f);
    SetWorldUp(glm::vec3(0.0f, 1.0f, 0.0f));

    UpdateCameraVectors();

    InputHelper::ProcessMouseButtons();
    InputHelper::ProcessMouseMovement();
}

glm::mat4 Camera::GetViewMatrix() const
{
    auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

    if (cameraIDs.size() <= 0)
        return glm::mat4(0.0);

    const EntityID& id = cameraIDs[0];

    if (!ECS::Get().HasComponent<Transform>(id))
        return glm::mat4(0.0);

    Transform& transform = *ECS::Get().GetComponent<Transform>(id);
    Position& pos = transform.position;
    return glm::lookAt(pos.GetWorld(), pos.GetWorld() + GetFront(), GetUp());
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(GetZoom()), GetAspectRatio(), GetNearPlane(), GetFarPlane());
}

void Camera::ProcessKeyboardMovement(Camera_Movement direction, float deltaTime)
{
    auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

    if (cameraIDs.size() <= 0)
        return;

    const EntityID& id = cameraIDs[0];

    if (!ECS::Get().HasComponent<Transform>(id) || !ECS::Get().HasComponent<Name>(id))
        return;

    float velocity = GetMovementSpeed() * deltaTime;

    auto& transform = *ECS::Get().GetComponent<Transform>(id);

    auto& pos = transform.position;
    auto& name = ECS::Get().GetComponent<Name>(id)->name;

    switch (direction) {
        case FORWARD:
            spdlog::info("{} move forward input detected by {:03.6f} unit.", name, velocity);
            pos.SetWorld(pos.GetWorld() + GetFront() * velocity);
            break;
        case BACKWARD:
            spdlog::info("{} move backward input detected by {:03.6f} unit.", name, velocity);
            pos.SetWorld(pos.GetWorld() - GetFront() * velocity);
            break;
        case RIGHT:
            spdlog::info("{} move right input detected by {:03.6f} unit.", name, velocity);
            pos.SetWorld(pos.GetWorld() + GetRight() * velocity);
            break;
        case LEFT:
            spdlog::info("{} move left input detected by {:03.6f} unit.", name, velocity);
            pos.SetWorld(pos.GetWorld() - GetRight() * velocity);
            break;
        case UP:
            spdlog::info("{} move up input detected by {:03.6f} unit.", name, velocity);
            pos.SetWorld(pos.GetWorld() + GetWorldUp() * velocity);
            break;
        case DOWN:
            spdlog::info("{} move down input detected by {:03.6f} unit.", name, velocity);
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
    glm::vec3 front;
    front.x = cos(glm::radians(GetYaw())) * cos(glm::radians(GetPitch()));
    front.y = sin(glm::radians(GetPitch()));
    front.z = sin(glm::radians(GetYaw())) * cos(glm::radians(GetPitch()));
    SetFront(glm::normalize(front));

    SetRight(glm::normalize(glm::cross(GetFront(), GetWorldUp())));
    SetUp(glm::normalize(glm::cross(GetRight(), GetFront())));
}
