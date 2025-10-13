#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Application/Core/Core.h>

namespace Nyx {

    class Scale {
    public:
        // Constructors
        Scale() : value(1.0f, 1.0f, 1.0f) {}

        explicit Scale(float uniform)
            : value(uniform, uniform, uniform) {
        }

        Scale(float x, float y, float z)
            : value(x, y, z) {}

        // Set new scale
        void set(float x, float y, float z) 
        {
            value = Math::Vec3f(x, y, z);
        }

        // Set uniform scale
        void setUniform(float uniform) 
        {
            value = Math::Vec3f(uniform);
        }

        // Return 4x4 scaling matrix
        Math::Mat4f ToMatrix() const 
        {
            return glm::scale(Math::Mat4f(1.0f), value);
        }

        // Return current scale vector
        Math::Vec3f& get()
        {
            return value;
        }

        float getRadius() const
        {
            return std::max(std::max(value.x, value.y), value.z);
        }

        /*
            OPERATOR OVERLOAD
        */

        // Scalar multiplication
        Scale operator*(float scalar) const {
            Math::Vec3f scaled = this->value * scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator/(float scalar) const {
            Math::Vec3f scaled = this->value / scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator+(float scalar) const {
            Math::Vec3f scaled = this->value + scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        Scale operator-(float scalar) const {
            Math::Vec3f scaled = this->value - scalar;
            return Scale(scaled.x, scaled.y, scaled.z);
        }

        // In-place
        Scale& operator*=(float scalar) {
            value *= scalar;
            return *this;
        }

        Scale& operator/=(float scalar) {
            value /= scalar;
            return *this;
        }

        Scale& operator+=(float scalar) {
            value += Math::Vec3f(scalar);
            return *this;
        }

        Scale& operator-=(float scalar) {
            value -= Math::Vec3f(scalar);
            return *this;
        }

    private:
        Math::Vec3f value;
    };

}