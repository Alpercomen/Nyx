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
		float intensity;

		Position position;
		Math::Vec3f direction;
		float range;
		float decay;
	};
}