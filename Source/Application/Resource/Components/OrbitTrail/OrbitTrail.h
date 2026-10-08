#pragma once
#include <Application/Core/Core.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
	struct OrbitTrail
	{
		bool8 enabled = false;

		float64 maxLength = 1.0e4;
		float64 sampleDistance = 1.0e-3;

		uint32 maxPoints = 2048;

		float32 fadePower = 20.0f;
		float32 opacity = 1.0f;

		Math::Vec3f color = { 1.0f, 1.0f, 1.0f };

		struct TrailPoint
		{
			Position position;
			float64 distance = 0.0;
			float32 alpha = 0.0f;
		};

		Deque<TrailPoint> points;
	};
}

