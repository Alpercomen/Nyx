//Camera.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

using namespace Nyx;

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct CameraDesc {
    Math::Vec3f Front = Math::Vec3f(0.0f, 0.0f, -1.0f);
    Math::Vec3f Up = Math::Vec3f(0.0f, 1.0f, 0.0f);
    Math::Vec3f Right = Math::Vec3f(1.0f, 0.0f, 0.0f);
    Math::Vec3f WorldUp = Math::Vec3f(0.0f, 1.0f, 0.0f);

    float32 Yaw = -90.0f;
    float32 Pitch = 0.0f;

    float32 MovementSpeed = METERS_PER_UNIT * 100;
    float32 MovementSpeedMultiplier = 3.5f;
    float32 MouseSensitivity = 0.1f;
    float32 Zoom = 45.0f;
    float32 AspectRatio = 1.7777f;

    float32 NearPlane = 0.01f;
    float32 FarPlane = (AU / METERS_PER_UNIT) * 2;
};

class Camera
{
public:

    Camera();
    ~Camera() = default;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    const Math::Vec3f& GetFront() const { return m_cameraDesc.Front; }
    const Math::Vec3f& GetUp() const { return m_cameraDesc.Up; }
    const Math::Vec3f& GetRight() const { return m_cameraDesc.Right; }
    const Math::Vec3f& GetWorldUp() const { return m_cameraDesc.WorldUp; }

    const float GetYaw() const { return m_cameraDesc.Yaw; }
    const float GetPitch() const { return m_cameraDesc.Pitch; }

    const float GetMovementSpeed() const { return m_cameraDesc.MovementSpeed; }
    const float GetMovementSpeedMultiplier() const { return m_cameraDesc.MovementSpeedMultiplier; }
    const float GetMouseSensitivity() const { return m_cameraDesc.MouseSensitivity; }
    const float GetZoom() const { return m_cameraDesc.Zoom; }
    const float GetAspectRatio() const { return m_cameraDesc.AspectRatio; }
    const float GetNearPlane() const { return m_cameraDesc.NearPlane; }
    const float GetFarPlane() const { return m_cameraDesc.FarPlane; }

    void SetFront(const Math::Vec3f& Front) { m_cameraDesc.Front = Front; }
    void SetUp(const Math::Vec3f& Up) { m_cameraDesc.Up = Up; }
    void SetRight(const Math::Vec3f& Right) { m_cameraDesc.Right = Right; }
    void SetWorldUp(const Math::Vec3f& WorldUp) { m_cameraDesc.WorldUp = WorldUp; }

    void SetYaw(const float32& Yaw) { m_cameraDesc.Yaw = Yaw; }
    void SetPitch(const float32& Pitch) { m_cameraDesc.Pitch = Pitch; }

    void SetMovementSpeed(const float32& MovementSpeed) { m_cameraDesc.MovementSpeed = MovementSpeed; }
    void SetMovementSpeedMultiplier(const float32& MovementSpeedMultiplier) { m_cameraDesc.MovementSpeedMultiplier = MovementSpeedMultiplier; }
    void SetMouseSensitivity(const float32& MouseSensitivity) { m_cameraDesc.MouseSensitivity = MouseSensitivity; }
    void SetZoom(const float32& Zoom) { m_cameraDesc.Zoom = Zoom; }
    void SetAspectRatio(const float32& AspectRatio) { m_cameraDesc.AspectRatio = AspectRatio; }
    void SetNearPlane(const float32& NearPlane) { m_cameraDesc.NearPlane = NearPlane; }
    void SetFarPlane(const float32& FarPlane) { m_cameraDesc.FarPlane = FarPlane; }

    void ProcessKeyboardMovement(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void UpdateCameraVectors();

    CameraDesc& GetCameraDesc() { return m_cameraDesc; }

private:
    CameraDesc m_cameraDesc;
};
