#pragma once
#include <Application/Core/Core.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Physics/Types.h>

namespace Nyx
{
	struct OrbitTrail
	{
		bool8 enabled = true;

		float32 trailFraction = 1.0f;

		uint32 segments = 512;

		float32 fadePower = 1.5f;
		float32 minAlpha = 0.0f;
		float32 opacity = 1.0f;

		Math::Vec3f color = { 1.0f, 1.0f, 1.0f };

		OrbitalElements orbit;
	};
}
