#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
	enum class LightType
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};

	class LightComponent
	{
	public: 
		LightType type;
		Math::Vec3f color;
		float64 intensity;

		Position position;
		Math::Vec3f direction;
		float64 range;
		float64 decay;
	};
}