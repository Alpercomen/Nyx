#include "Camera.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <Application/Core/Core.h>
#include <Application/Window/Window.h>
#include <Application/Constants/Constants.h>
#include <Application/Services/Input/InputDispatcher.h>
#include <Application/Services/Input/InputEvent.h>
#include <Application/Services/Input/InputQueue.h>
#include <Application/Services/Camera/CameraService.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Services/Editor/Editor.h>

Camera::Camera()
{
    SetFront(Math::Vec3f(0.0f, 0.0f, -1.0f));
    SetMovementSpeed(0.3);
    SetMovementSpeedMultiplier(5.f);
    SetMouseSensitivity(0.1f);
    SetZoom(45.0f);
    SetYaw(-90.0f);
    SetPitch(0.0f);
    SetWorldUp(Math::Vec3f(0.0f, 1.0f, 0.0f));

    UpdateCameraVectors();

    InputHelper::ProcessMouseButtons();
    InputHelper::ProcessMouseMovement();
    InputHelper::ProcessMouseScroll();
}

glm::mat4 Camera::GetViewMatrix() const
{
    auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

    if (cameraIDs.size() <= 0)
        return Math::Mat4d(0.0);

    const EntityID& id = cameraIDs[0];

    if (!ECS::Get().HasComponent<Transform>(id))
        return Math::Mat4d(0.0);

    auto& camera = *ECS::Get().GetComponent<Camera>(id);

    if (CameraService::Get().enabled && Editor::Get().selectedEntity.has_value())
        LockCamera(camera, id);

    camera.UpdateTrackingClipPlanes(CameraService::Get().focusEnabled);

    Transform& transform = *ECS::Get().GetComponent<Transform>(id);
    Position& pos = transform.position;

    return glm::lookAt(Math::Vec3d(0.0), GetFront(), GetUp());
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(GetZoom()), GetAspectRatio(), GetNearPlane(), GetFarPlane());
}

void Camera::ProcessKeyboardMovement(Camera_Movement direction, float64 deltaTime)
{
    auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

    if (cameraIDs.size() <= 0)
        return;

    const EntityID& id = cameraIDs[0];

    if (!ECS::Get().HasComponent<Transform>(id) || !ECS::Get().HasComponent<Name>(id))
        return;

    float64 velocity = GetMovementSpeed() * deltaTime;

    auto& transform = *ECS::Get().GetComponent<Transform>(id);

    auto& pos = transform.position;
    auto& name = ECS::Get().GetComponent<Name>(id)->name;

    switch (direction)
    {
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

void Camera::ProcessMouseMovement(float64 xoffset, float64 yoffset, bool constrainPitch)
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

void Camera::LockCamera(Camera& camera, const EntityID& id) const
{
    const EntityID& targetID = CameraService::Get().targetEntity;

    if (ECS::Get().HasComponent<Transform>(targetID))
    {
        auto& targetTransform = *ECS::Get().GetComponent<Transform>(targetID);
        auto& cameraTransform = *ECS::Get().GetComponent<Transform>(id);

        const Position& pos = targetTransform.position / METERS_PER_UNIT;
        Math::Vec3d targetPos = pos.GetWorld();

        const float64 size = glm::length(targetTransform.scale.get()) / METERS_PER_UNIT;
        const float64 focusRadius = CameraService::Get().focusRadius / METERS_PER_UNIT;

        CameraService::Get().minimumDistance = size * CAMERA_MINIMUM_ZOOM_MULTIPLIER;
        CameraService::Get().maximumDistance = size * CAMERA_MAXIMUM_ZOOM_MULTIPLIER;

        CameraService::Get().distance = glm::clamp(
            CameraService::Get().distance,
            CameraService::Get().minimumDistance,
            CameraService::Get().maximumDistance
        );

        if (size <= focusRadius)
            CameraService::Get().focusEnabled = true;
        else
            CameraService::Get().focusEnabled = false;

        const float64 distance = CameraService::Get().distance;

        if (CameraService::Get().lockOrientation)
        {
            Math::Quatd targetRot = targetTransform.rotation.GetQuaternion();

            Math::Vec3d targetRight = glm::normalize(targetRot * Math::Vec3d(1.0, 0.0, 0.0));
            Math::Vec3d targetUp = glm::normalize(targetRot * Math::Vec3d(0.0, 1.0, 0.0));
            Math::Vec3d targetForward = glm::normalize(targetRot * Math::Vec3d(0.0, 0.0, 1.0));

            float64 yaw = CameraService::Get().yaw;
            float64 pitch = CameraService::Get().pitch;
            float64 distance = CameraService::Get().distance;

            Math::Vec3d localDirection;
            localDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            localDirection.y = sin(glm::radians(pitch));
            localDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            localDirection = glm::normalize(localDirection);

            Math::Vec3d worldDirection =
                targetRight * localDirection.x +
                targetUp * localDirection.y +
                targetForward * localDirection.z;

            worldDirection = glm::normalize(worldDirection);

            Math::Vec3d cameraPos = targetPos - worldDirection * distance;
            cameraTransform.position.SetWorld(cameraPos);

            Math::Vec3d front = glm::normalize(targetPos - cameraPos);
            Math::Vec3d right = glm::cross(front, targetUp);

            if (glm::length2(right) < 1e-12)
            {
                right = glm::cross(front, targetRight);
            }

            right = glm::normalize(right);
            Math::Vec3d up = glm::normalize(glm::cross(right, front));

            camera.SetFront(front);
            camera.SetRight(right);
            camera.SetUp(up);
        }
        else
        {
            float64 yaw = CameraService::Get().yaw;
            float64 pitch = CameraService::Get().pitch;

            Math::Vec3d direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction = glm::normalize(direction);

            Math::Vec3d cameraPos = targetPos - direction * distance;
            cameraTransform.position.SetWorld(cameraPos);

            camera.SetFront(glm::normalize(targetPos - cameraPos));
            camera.SetRight(glm::normalize(glm::cross(camera.GetFront(), camera.GetWorldUp())));
            camera.SetUp(glm::normalize(glm::cross(camera.GetRight(), camera.GetFront())));
        }
    }
}
