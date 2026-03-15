#pragma once

#include <Application/Core/Core.h>

namespace Nyx
{
	struct Vertex
	{
		Math::Vec3f position;
		Math::Vec3f normal;
		Math::Vec3f tangent;
		Math::Vec2f texCoord;
	};
}