//Camera.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

using namespace Nyx;

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct CameraDesc
{
    Math::Vec3d Front = Math::Vec3d(0.0f, 0.0f, -1.0f);
    Math::Vec3d Up = Math::Vec3d(0.0f, 1.0f, 0.0f);
    Math::Vec3d Right = Math::Vec3d(1.0f, 0.0f, 0.0f);
    Math::Vec3d WorldUp = Math::Vec3d(0.0f, 1.0f, 0.0f);

    float64 Yaw = -90.0;
    float64 Pitch = 0.0;

    float64 MovementSpeed = METERS_PER_UNIT * 100;
    float64 MovementSpeedMultiplier = 3.5;
    float64 MouseSensitivity = 0.1;
    float64 Zoom = 45.0;
    float64 AspectRatio = 1.7777;

    float64 NearPlane = 0.1;
    float64 FarPlane = 10000;
};

class Camera
{
public:

    Camera();
    ~Camera() = default;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    const Math::Vec3d& GetFront() const { return m_cameraDesc.Front; }
    const Math::Vec3d& GetUp() const { return m_cameraDesc.Up; }
    const Math::Vec3d& GetRight() const { return m_cameraDesc.Right; }
    const Math::Vec3d& GetWorldUp() const { return m_cameraDesc.WorldUp; }

    const float64 GetYaw() const { return m_cameraDesc.Yaw; }
    const float64 GetPitch() const { return m_cameraDesc.Pitch; }

    const float64 GetMovementSpeed() const { return m_cameraDesc.MovementSpeed; }
    const float64 GetMovementSpeedMultiplier() const { return m_cameraDesc.MovementSpeedMultiplier; }
    const float64 GetMouseSensitivity() const { return m_cameraDesc.MouseSensitivity; }
    const float64 GetZoom() const { return m_cameraDesc.Zoom; }
    const float64 GetAspectRatio() const { return m_cameraDesc.AspectRatio; }
    const float64 GetNearPlane() const { return m_cameraDesc.NearPlane; }
    const float64 GetFarPlane() const { return m_cameraDesc.FarPlane; }

    void SetFront(const Math::Vec3d& Front) { m_cameraDesc.Front = Front; }
    void SetUp(const Math::Vec3d& Up) { m_cameraDesc.Up = Up; }
    void SetRight(const Math::Vec3d& Right) { m_cameraDesc.Right = Right; }
    void SetWorldUp(const Math::Vec3d& WorldUp) { m_cameraDesc.WorldUp = WorldUp; }

    void SetYaw(const float64& Yaw) { m_cameraDesc.Yaw = Yaw; }
    void SetPitch(const float64& Pitch) { m_cameraDesc.Pitch = Pitch; }

    void SetMovementSpeed(const float64& MovementSpeed) { m_cameraDesc.MovementSpeed = MovementSpeed; }
    void SetMovementSpeedMultiplier(const float64& MovementSpeedMultiplier) { m_cameraDesc.MovementSpeedMultiplier = MovementSpeedMultiplier; }
    void SetMouseSensitivity(const float64& MouseSensitivity) { m_cameraDesc.MouseSensitivity = MouseSensitivity; }
    void SetZoom(const float64& Zoom) { m_cameraDesc.Zoom = Zoom; }
    void SetAspectRatio(const float64& AspectRatio) { m_cameraDesc.AspectRatio = AspectRatio; }
    void SetNearPlane(const float64& NearPlane) { m_cameraDesc.NearPlane = NearPlane; }
    void SetFarPlane(const float64& FarPlane) { m_cameraDesc.FarPlane = FarPlane; }

    void ProcessKeyboardMovement(Camera_Movement direction, float64 deltaTime);
    void ProcessMouseMovement(float64 xoffset, float64 yoffset, bool constrainPitch = true);
    void UpdateCameraVectors();

private:
    CameraDesc m_cameraDesc;
};
