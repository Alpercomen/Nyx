#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Application/Core/Core.h>

namespace Nyx {

    class Scale {
    public:
        // Constructors
        Scale() : m_scale(1.0f, 1.0f, 1.0f) {}

        explicit Scale(float uniform)
            : m_scale(uniform, uniform, uniform) {
        }

        Scale(float x, float y, float z)
            : m_scale(x, y, z) {
        }

        // Set new scale
        void set(float x, float y, float z) {
            m_scale = Math::Vec3f(x, y, z);
        }

        // Set uniform scale
        void setUniform(float value) {
            m_scale = Math::Vec3f(value);
        }

        // Return 4x4 scaling matrix
        Math::Mat4f ToMatrix() const {
            return glm::scale(Math::Mat4f(1.0f), m_scale);
        }

        // Return current scale vector
        Math::Vec3f get() const {
            return m_scale;
        }

        /*
            OPERATOR OVERLOAD
        */

        // Scalar multiplication
        Scale operator*(float scalar) const {
            Math::Vec3f scaled = this->m_scale * scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator/(float scalar) const {
            Math::Vec3f scaled = this->m_scale / scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator+(float scalar) const {
            Math::Vec3f scaled = this->m_scale + scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator-(float scalar) const {
            Math::Vec3f scaled = this->m_scale - scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        // In-place
        Scale& operator*=(float scalar) {
            m_scale *= scalar;
            return *this;
        }

        Scale& operator/=(float scalar) {
            m_scale /= scalar;
            return *this;
        }

        Scale& operator+=(float scalar) {
            m_scale += Math::Vec3f(scalar);
            return *this;
        }

        Scale& operator-=(float scalar) {
            m_scale -= Math::Vec3f(scalar);
            return *this;
        }

    private:
        Math::Vec3f m_scale;
    };

}