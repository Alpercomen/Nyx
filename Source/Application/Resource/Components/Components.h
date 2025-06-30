#pragma once

#include <Application/Resource/Components/Transform/Position.h>
#include <Application/Resource/Components/Transform/Rotation.h>
#include <Application/Resource/Components/Transform/Scale.h>
#include <Application/Resource/Components/Rigidbody/Velocity.h>
#include <Application/Resource/Components/Rigidbody/Acceleration.h>
#include <Application/Resource/Components/Mesh/Mesh.h>
#include <Application/Resource/Components/Mesh/GridMesh/GridMesh.h>

namespace Nyx
{
	struct Name
	{
		String name;
	};

	struct Transform
	{
		Position position;
		Rotation rotation;
		Scale scale;
	};

	struct Rigidbody
	{
		float64 mass;
		Velocity velocity;
		Acceleration acceleration;
	};

}