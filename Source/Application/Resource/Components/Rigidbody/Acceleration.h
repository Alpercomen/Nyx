#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Application/Constants/Constants.h>

class Acceleration {
public:
	// CTOR
	Acceleration()
	{
		m_world = glm::vec3();
		m_normalized = glm::vec3();
	}

	Acceleration(glm::vec3 acceleration, bool normal = false)
	{
		normal ? SetNormal(acceleration) : SetWorld(acceleration);
	}

	~Acceleration() = default;

	// Getters
	const glm::vec3& GetWorld() const { return m_world; }
	const glm::vec3& GetNormal() const { return m_normalized; }

	// Setters
	void SetWorld(const glm::vec3& acceleration)
	{
		m_world = acceleration;

		m_normalized.x = acceleration.x / METERS_PER_UNIT;
		m_normalized.y = acceleration.y / METERS_PER_UNIT;
		m_normalized.z = acceleration.z / METERS_PER_UNIT;
	}

	void SetNormal(const glm::vec3& acceleration)
	{
		m_world.x = acceleration.x * METERS_PER_UNIT;
		m_world.y = acceleration.y * METERS_PER_UNIT;
		m_world.z = acceleration.z * METERS_PER_UNIT;

		m_normalized = acceleration;
	}

private:
	glm::vec3 m_world;
	glm::vec3 m_normalized;
};