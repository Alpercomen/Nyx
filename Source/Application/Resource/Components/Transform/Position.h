#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Application/Constants/Constants.h>
#include <Application/Core/Core.h>

using namespace Math;

class Position {
public:
	Position()
	{
		m_world = Math::Vec3f();
		m_normalized = Math::Vec3f();
	}

	Position(Math::Vec3f position, bool normal = false)
	{
		normal ? SetNormal(position) : SetWorld(position);
	}

	~Position() = default;

	// Getters
	Math::Vec3f& GetWorld() { return m_world; }
	Math::Vec3f& GetNormal() { return m_normalized; }

	const Math::Vec3f& GetWorld() const { return m_world; }
	const Math::Vec3f& GetNormal() const { return m_normalized; }

	// Setters
	void SetWorld(const Math::Vec3f& position)
	{
		m_world = position;

		m_normalized.x = position.x / METERS_PER_UNIT;
		m_normalized.y = position.y / METERS_PER_UNIT;
		m_normalized.z = position.z / METERS_PER_UNIT;
	}

	void SetNormal(const Math::Vec3f& position)
	{
		m_world.x = position.x * METERS_PER_UNIT;
		m_world.y = position.y * METERS_PER_UNIT;
		m_world.z = position.z * METERS_PER_UNIT;

		m_normalized = position;
	}

	double distance3D(Position other)
	{
		return glm::distance(this->GetWorld(), other.GetWorld());
	}

	/*
		OPERATOR OVERLOAD
	*/

	// Scalar multiplication
	Position operator*(float scalar) const {
		return Position(this->m_world * scalar);
	}

	// Scalar division
	Position operator/(float scalar) const {
		return Position(this->m_world / scalar);
	}

	// Scalar addition
	Position operator+(float scalar) const {
		return Position(this->m_world + glm::vec3(scalar));
	}

	// Scalar subtraction
	Position operator-(float scalar) const {
		return Position(this->m_world - glm::vec3(scalar));
	}

	// In-place versions (e.g., pos *= 2)
	Position& operator*=(float scalar) {
		this->SetWorld(this->m_world * scalar);
		return *this;
	}

	Position& operator/=(float scalar) {
		this->SetWorld(this->m_world / scalar);
		return *this;
	}

	Position& operator+=(float scalar) {
		this->SetWorld(this->m_world + glm::vec3(scalar));
		return *this;
	}

	Position& operator-=(float scalar) {
		this->SetWorld(this->m_world - glm::vec3(scalar));
		return *this;
	}

private:
	Math::Vec3f m_world;
	Math::Vec3f m_normalized;
};