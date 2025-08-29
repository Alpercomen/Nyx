#pragma once
#include <glm/gtx/euler_angles.hpp>

#include <Application/Resource/Components/Transform/Position.h>
#include <Application/Resource/Components/Transform/Rotation.h>
#include <Application/Resource/Components/Transform/Scale.h>
#include <Application/Resource/Components/Rigidbody/Velocity.h>
#include <Application/Resource/Components/Rigidbody/Acceleration.h>
#include <Application/Resource/Components/Mesh/Mesh.h>
#include <Application/Resource/Components/Mesh/GridMesh/GridMesh.h>
#include <Application/Resource/Components/Lighting/Light.h>

namespace Nyx
{
	struct Name
	{
		String name;
	};

	struct Transform
	{
	public:
		Position position;
		Rotation rotation;
		Scale scale;
	};

	struct Rigidbody
	{
		float64 mass;
		Velocity velocity;
		Velocity angularVelocity;
		Acceleration acceleration;
	};

	struct TidallyLocked
	{
		TidallyLocked() = default;
		TidallyLocked(EntityID lockedEntity) : lockedEntity(lockedEntity) {}

		EntityID lockedEntity;
	};

	class FixedRotation
	{
	public:

		FixedRotation(Math::Vec3d axis, float rotateDegrees)
		{
			this->axis = axis;
			this->rotateDegrees = rotateDegrees;
		}

		Math::Vec3d GetAxis() { return axis; }
		float GetDegrees() { return rotateDegrees; }
		float GetRadians() { return glm::radians(rotateDegrees); }

	private:
		Math::Vec3d axis;
		float rotateDegrees;

	};

}