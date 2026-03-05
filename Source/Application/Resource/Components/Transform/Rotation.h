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
        Rotation() : m_rotation(glm::quat_identity<double, glm::qualifier::defaultp>()) {}

        explicit Rotation(const Math::Quatd& q)
            : m_rotation(glm::normalize(q)) {
        }

        Rotation(double pitch, double yaw, double roll) {
            SetFromEuler(pitch, yaw, roll);
        }

        // Set rotation from Euler angles (pitch, yaw, roll)
        void SetFromEuler(double pitch, double yaw, double roll) {
            m_rotation = Math::Quatd(Math::Vec3d(pitch, yaw, roll));
        }

        // Set rotation from quaternion
        void SetQuaternion(const Math::Quatd& q) {
            m_rotation = glm::normalize(q);
        }

        // Convert to 4x4 rotation matrix
        Math::Mat4d ToMatrix() const {
            return glm::toMat4(m_rotation);
        }

        // Access quaternion
        Math::Quatd GetQuaternion() const {
            return m_rotation;
        }

        // Return Euler angles in radians
        Math::Vec3d GetEulerAngles() const {
            return glm::eulerAngles(m_rotation);
        }

        // Rotate around world-space axis by angle (radians)
        void Rotate(const Math::Vec3d& axis, double angleRadians) {
            Math::Quatd delta = glm::angleAxis(angleRadians, glm::normalize(Math::Vec3d(axis)));
            m_rotation = glm::normalize(delta * m_rotation);
        }

        // Rotate around local pitch/yaw/roll (in radians)
        void RotateLocal(double yaw, double pitch, double roll) {
            Math::Quatd delta = Math::Quatd(Math::Vec3d(pitch, yaw, roll));
            m_rotation = glm::normalize(m_rotation * delta);
        }

        // Get the forward vector (Z-)
        Math::Vec3d GetForward() const {
            return glm::rotate(m_rotation, Math::Vec3d(0.0f, 0.0f, -1.0f));
        }

        // Get the up vector (Y+)
        Math::Vec3d GetUp() const {
            return glm::rotate(m_rotation, Math::Vec3d(0.0f, 1.0f, 0.0f));
        }

        // Get the right vector (X+)
        Math::Vec3d GetRight() const {
            return glm::rotate(m_rotation, Math::Vec3d(1.0f, 0.0f, 0.0f));
        }

        /*
            OPERATOR OVERLOAD
        */

        Rotation operator*(double scalar) const {
            auto euler = glm::eulerAngles(this->m_rotation) * scalar;
            Rotation result;
            result.SetFromEuler(euler.x, euler.y, euler.z);
            return result;
        }

        Rotation operator/(double scalar) const {
            auto euler = glm::eulerAngles(this->m_rotation) / scalar;
            Rotation result;
            result.SetFromEuler(euler.x, euler.y, euler.z);
            return result;
        }

    private:
        Math::Quatd m_rotation;
    };

}