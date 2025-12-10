#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Rigidbody/Acceleration.h>

class Velocity {
public:
	// CTOR
	Velocity()
	{
		m_world = glm::vec3();
		m_normalized = glm::vec3();
	}

	Velocity(glm::vec3 velocity, bool normal = false)
	{
		normal ? SetNormal(velocity) : SetWorld(velocity);
	}

	~Velocity() = default;

	// Getters
	const glm::vec3& GetWorld() const { return m_world; }
	const glm::vec3& GetNormal() const { return m_normalized; }

	// Setters
	void SetWorld(const glm::vec3& velocity)
	{
		m_world = velocity;

		m_normalized.x = velocity.x / METERS_PER_UNIT;
		m_normalized.y = velocity.y / METERS_PER_UNIT;
		m_normalized.z = velocity.z / METERS_PER_UNIT;
	}

	void SetNormal(const glm::vec3& velocity)
	{
		m_world.x = velocity.x * METERS_PER_UNIT;
		m_world.y = velocity.y * METERS_PER_UNIT;
		m_world.z = velocity.z * METERS_PER_UNIT;

		m_normalized = velocity;
	}

	void Accelerate(Acceleration& acceleration)
	{
		// Physics update in meters
		double nextX = GetWorld().x + acceleration.GetWorld().x * DELTA_TIME * TIME_SCALE;
		double nextY = GetWorld().y + acceleration.GetWorld().y * DELTA_TIME * TIME_SCALE;
		double nextZ = GetWorld().z + acceleration.GetWorld().z * DELTA_TIME * TIME_SCALE;

		SetWorld(glm::vec3(nextX, nextY, nextZ));
	}

private:
	glm::vec3 m_world;
	glm::vec3 m_normalized;
};