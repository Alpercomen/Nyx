#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Application/Core/Core.h>

namespace Nyx {

    class Scale {
    public:
        // Constructors
        Scale() : m_scale(1.0f, 1.0f, 1.0f) {}

        explicit Scale(double uniform)
            : m_scale(uniform, uniform, uniform) {
        }

        Scale(double x, double y, double z)
            : m_scale(x, y, z) {
        }

        // Set new scale
        void set(double x, double y, double z) {
            m_scale = Math::Vec3d(x, y, z);
        }

        // Set uniform scale
        void setUniform(double value) {
            m_scale = Math::Vec3d(value);
        }

        // Return 4x4 scaling matrix
        Math::Mat4d ToMatrix() const {
            return glm::scale(Math::Mat4d(1.0f), m_scale);
        }

        // Return current scale vector
        Math::Vec3d get() const {
            return m_scale;
        }

        /*
            OPERATOR OVERLOAD
        */

        // Scalar multiplication
        Scale operator*(double scalar) const {
            Math::Vec3d scaled = this->m_scale * scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator/(double scalar) const {
            Math::Vec3d scaled = this->m_scale / scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator+(double scalar) const {
            Math::Vec3d scaled = this->m_scale + scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator-(double scalar) const {
            Math::Vec3d scaled = this->m_scale - scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        // In-place
        Scale& operator*=(double scalar) {
            m_scale *= scalar;
            return *this;
        }

        Scale& operator/=(double scalar) {
            m_scale /= scalar;
            return *this;
        }

        Scale& operator+=(double scalar) {
            m_scale += Math::Vec3d(scalar);
            return *this;
        }

        Scale& operator-=(double scalar) {
            m_scale -= Math::Vec3d(scalar);
            return *this;
        }

    private:
        Math::Vec3d m_scale;
    };

}