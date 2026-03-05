#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Application/Constants/Constants.h>

class Acceleration {
public:
	// CTOR
	Acceleration()
	{
		m_world = Math::Vec3d();
		m_normalized = Math::Vec3d();
	}

	Acceleration(Math::Vec3d acceleration, bool normal = false)
	{
		normal ? SetNormal(acceleration) : SetWorld(acceleration);
	}

	~Acceleration() = default;

	// Getters
	const Math::Vec3d& GetWorld() const { return m_world; }
	const Math::Vec3d& GetNormal() const { return m_normalized; }

	// Setters
	void SetWorld(const Math::Vec3d& acceleration)
	{
		m_world = acceleration;

		m_normalized.x = acceleration.x / METERS_PER_UNIT;
		m_normalized.y = acceleration.y / METERS_PER_UNIT;
		m_normalized.z = acceleration.z / METERS_PER_UNIT;
	}

	void SetNormal(const Math::Vec3d& acceleration)
	{
		m_world.x = acceleration.x * METERS_PER_UNIT;
		m_world.y = acceleration.y * METERS_PER_UNIT;
		m_world.z = acceleration.z * METERS_PER_UNIT;

		m_normalized = acceleration;
	}

private:
	Math::Vec3d m_world;
	Math::Vec3d m_normalized;
};