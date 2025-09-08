#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Application/Core/Core.h>

namespace Nyx {

    class Rotation {
    public:
        // Constructors
        Rotation() : m_rotation(glm::quat_identity<float, glm::qualifier::defaultp>()) {}

        explicit Rotation(const Math::Quatf& q)
            : m_rotation(glm::normalize(q)) {
        }

        Rotation(float pitch, float yaw, float roll) {
            SetFromEuler(pitch, yaw, roll);
        }

        // Set rotation from Euler angles (pitch, yaw, roll)
        void SetFromEuler(float pitch, float yaw, float roll) {
            m_rotation = Math::Quatf(Math::Vec3f(pitch, yaw, roll));
        }

        // Set rotation from quaternion
        void SetQuaternion(const Math::Quatf& q) {
            m_rotation = glm::normalize(q);
        }

        // Convert to 4x4 rotation matrix
        Math::Mat4f ToMatrix() const {
            return glm::toMat4(m_rotation);
        }

        // Access quaternion
        Math::Quatf GetQuaternion() const {
            return m_rotation;
        }

        // Return Euler angles in radians
        Math::Vec3f GetEulerAngles() const {
            return glm::eulerAngles(m_rotation);
        }

        // Rotate around world-space axis by angle (radians)
        void Rotate(const Math::Vec3d& axis, float angleRadians) {
            Math::Quatf delta = glm::angleAxis(angleRadians, glm::normalize(Math::Vec3f(axis)));
            m_rotation = glm::normalize(delta * m_rotation);
        }

        // Rotate around local pitch/yaw/roll (in radians)
        void RotateLocal(float yaw, float pitch, float roll) {
            Math::Quatf delta = Math::Quatf(Math::Vec3f(pitch, yaw, roll));
            m_rotation = glm::normalize(m_rotation * delta);
        }

        // Get the forward vector (Z-)
        Math::Vec3f GetForward() const {
            return glm::rotate(m_rotation, Math::Vec3f(0.0f, 0.0f, -1.0f));
        }

        // Get the up vector (Y+)
        Math::Vec3f GetUp() const {
            return glm::rotate(m_rotation, Math::Vec3f(0.0f, 1.0f, 0.0f));
        }

        // Get the right vector (X+)
        Math::Vec3f GetRight() const {
            return glm::rotate(m_rotation, Math::Vec3f(1.0f, 0.0f, 0.0f));
        }

        /*
            OPERATOR OVERLOAD
        */

        Rotation operator*(float scalar) const {
            auto euler = glm::eulerAngles(this->m_rotation) * scalar;
            Rotation result;
            result.SetFromEuler(euler.x, euler.y, euler.z);
            return result;
        }

        Rotation operator/(float scalar) const {
            auto euler = glm::eulerAngles(this->m_rotation) / scalar;
            Rotation result;
            result.SetFromEuler(euler.x, euler.y, euler.z);
            return result;
        }

    private:
        Math::Quatf m_rotation;
    };

}