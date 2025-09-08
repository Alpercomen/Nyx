#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Application/Constants/Constants.h>

class Acceleration {
public:
	// CTOR
	Acceleration()
	{
		m_world = Math::Vec3f();
		m_normalized = Math::Vec3f();
	}

	Acceleration(Math::Vec3f acceleration, bool normal = false)
	{
		normal ? SetNormal(acceleration) : SetWorld(acceleration);
	}

	~Acceleration() = default;

	// Getters
	const Math::Vec3f& GetWorld() const { return m_world; }
	const Math::Vec3f& GetNormal() const { return m_normalized; }

	// Setters
	void SetWorld(const Math::Vec3f& acceleration)
	{
		m_world = acceleration;

		m_normalized.x = acceleration.x / METERS_PER_UNIT;
		m_normalized.y = acceleration.y / METERS_PER_UNIT;
		m_normalized.z = acceleration.z / METERS_PER_UNIT;
	}

	void SetNormal(const Math::Vec3f& acceleration)
	{
		m_world.x = acceleration.x * METERS_PER_UNIT;
		m_world.y = acceleration.y * METERS_PER_UNIT;
		m_world.z = acceleration.z * METERS_PER_UNIT;

		m_normalized = acceleration;
	}

private:
	Math::Vec3f m_world;
	Math::Vec3f m_normalized;
};