#pragma once
#include <Application/Resource/Components/Transform/Position.h>
#include <Application/Resource/Components/Transform/Rotation.h>
#include <Application/Resource/Components/Transform/Scale.h>

namespace Nyx
{
	struct Transform
	{
		Position position;
		Rotation rotation;
		Scale scale;
	};
}