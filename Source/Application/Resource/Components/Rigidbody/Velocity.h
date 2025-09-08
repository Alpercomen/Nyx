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
		m_world = Math::Vec3f();
		m_normalized = Math::Vec3f();
	}

	Velocity(Math::Vec3f velocity, bool normal = false)
	{
		normal ? SetNormal(velocity) : SetWorld(velocity);
	}

	~Velocity() = default;

	// Getters
	const Math::Vec3f& GetWorld() const { return m_world; }
	const Math::Vec3f& GetNormal() const { return m_normalized; }

	// Setters
	void SetWorld(const Math::Vec3f& velocity)
	{
		m_world = velocity;

		m_normalized.x = velocity.x / METERS_PER_UNIT;
		m_normalized.y = velocity.y / METERS_PER_UNIT;
		m_normalized.z = velocity.z / METERS_PER_UNIT;
	}

	void SetNormal(const Math::Vec3f& velocity)
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

		SetWorld(Math::Vec3f(nextX, nextY, nextZ));
	}

private:
	Math::Vec3f m_world;
	Math::Vec3f m_normalized;
};