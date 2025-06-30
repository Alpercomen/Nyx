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
            setFromEuler(pitch, yaw, roll);
        }

        // Set rotation from Euler angles (pitch, yaw, roll)
        void setFromEuler(float pitch, float yaw, float roll) {
            m_rotation = Math::Quatf(Math::Vec3f(pitch, yaw, roll));
        }

        // Set rotation from quaternion
        void setQuaternion(const Math::Quatf& q) {
            m_rotation = glm::normalize(q);
        }

        // Convert to 4x4 rotation matrix
        Math::Mat4f toMatrix() const {
            return glm::toMat4(m_rotation);
        }

        // Access quaternion
        Math::Quatf getQuaternion() const {
            return m_rotation;
        }

        // Return Euler angles in radians
        Math::Vec3f getEulerAngles() const {
            return glm::eulerAngles(m_rotation);
        }

        // Rotate around world-space axis by angle (radians)
        void rotate(const Math::Vec3d& axis, float angleRadians) {
            Math::Quatf delta = glm::angleAxis(angleRadians, glm::normalize(glm::vec3(axis)));
            m_rotation = glm::normalize(delta * m_rotation);
        }

        // Rotate around local pitch/yaw/roll (in radians)
        void rotateLocal(float pitch, float yaw, float roll) {
            Math::Quatf delta = Math::Quatf(Math::Vec3f(pitch, yaw, roll));
            m_rotation = glm::normalize(m_rotation * delta);
        }

    private:
        Math::Quatf m_rotation;
    };

}