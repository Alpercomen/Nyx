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

	double distance3D(const Position& other) const
	{
		return glm::distance(this->GetWorld(), other.GetWorld());
	}

private:
	Math::Vec3f m_world;
	Math::Vec3f m_normalized;
};