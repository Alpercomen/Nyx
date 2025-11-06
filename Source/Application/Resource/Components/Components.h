#pragma once
#include <glm/gtx/euler_angles.hpp>
#include <Application/Resource/Components/Transform/Transform.h>
#include <Application/Resource/Components/Rigidbody/Velocity.h>
#include <Application/Resource/Components/Rigidbody/Acceleration.h>
#include <Application/Resource/Components/Mesh/Mesh.h>
#include <Application/Resource/Components/Mesh/GridMesh/GridMesh.h>
#include <Application/Resource/Components/Effects/Atmosphere.h>

namespace Nyx
{
	struct Name
	{
		String name;
	};

	struct Rigidbody
	{
		float64 mass;
		Velocity angularVelocity;
		Velocity velocity;
		Acceleration acceleration;
	};

	struct TidallyLocked
	{
		TidallyLocked() = default;
		TidallyLocked(EntityID lockedEntity) : lockedEntity(lockedEntity) {}

		EntityID lockedEntity;
	};

}