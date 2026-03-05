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
		m_world = Math::Vec3d();
		m_normalized = Math::Vec3d();
	}

	Position(Math::Vec3d position, bool normal = false)
	{
		normal ? SetNormal(position) : SetWorld(position);
	}

	~Position() = default;

	// Getters
	const Math::Vec3d& GetWorld() const { return m_world; }
	const Math::Vec3d& GetNormal() const { return m_normalized; }

	// Setters
	void SetWorld(const Math::Vec3d& position)
	{
		m_world = position;

		m_normalized.x = position.x / METERS_PER_UNIT;
		m_normalized.y = position.y / METERS_PER_UNIT;
		m_normalized.z = position.z / METERS_PER_UNIT;
	}

	void SetNormal(const Math::Vec3d& position)
	{
		m_world.x = position.x * METERS_PER_UNIT;
		m_world.y = position.y * METERS_PER_UNIT;
		m_world.z = position.z * METERS_PER_UNIT;

		m_normalized = position;
	}

	double distance3D(const Position& other) const
	{
		return glm::distance(this->GetWorld(), other.GetWorld());
	}

	/*
		OPERATOR OVERLOAD
	*/

	// Scalar multiplication
	Position operator*(double scalar) const {
		return Position(this->m_world * scalar);
	}

	// Scalar division
	Position operator/(double scalar) const {
		return Position(this->m_world / scalar);
	}

	// Scalar addition
	Position operator+(double scalar) const {
		return Position(this->m_world + Math::Vec3d(scalar));
	}

	// Scalar subtraction
	Position operator-(double scalar) const {
		return Position(this->m_world - Math::Vec3d(scalar));
	}

	// In-place versions
	Position& operator*=(double scalar) {
		this->SetWorld(this->m_world * scalar);
		return *this;
	}

	Position& operator/=(double scalar) {
		this->SetWorld(this->m_world / scalar);
		return *this;
	}

	Position& operator+=(double scalar) {
		this->SetWorld(this->m_world + Math::Vec3d(scalar));
		return *this;
	}

	Position& operator-=(double scalar) {
		this->SetWorld(this->m_world - Math::Vec3d(scalar));
		return *this;
	}

private:
	Math::Vec3d m_world;
	Math::Vec3d m_normalized;
};