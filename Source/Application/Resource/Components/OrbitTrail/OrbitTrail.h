#pragma once
#include <Application/Core/Core.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
	struct OrbitTrail
	{
		bool8 enabled = true;

		// --- Appearance ---

		// Fraction of the full orbit drawn behind the body, in (0, 1]. 1.0 = whole ellipse.
		float32 trailFraction = 1.0f;

		// Number of line segments used to draw the visible arc.
		uint32 segments = 512;

		// Fade shape along the arc. 1 = linear, higher = shorter bright tail.
		float32 fadePower = 1.5f;

		// Alpha floor at the far end of the arc. 0 = fully faded out, >0 keeps a faint ring.
		float32 minAlpha = 0.0f;

		float32 opacity = 1.0f;

		Math::Vec3f color = { 1.0f, 1.0f, 1.0f };

		// --- Cached orbit (written every frame by OrbitTrailSystem::Update) ---
		// Everything here is in meters, relative to the orbital parent.
		struct Orbit
		{
			bool8 valid = false;

			EntityID parentID = NO_ID;

			float64 semiMajor = 0.0;
			float64 semiMinor = 0.0;
			float64 eccentricity = 0.0;

			// Eccentric anomaly of the body right now, in radians.
			float64 currentAnomaly = 0.0;

			// Perifocal basis: P points to periapsis, Q is 90 degrees ahead in the direction of motion.
			Math::Vec3d P = Math::Vec3d(1.0, 0.0, 0.0);
			Math::Vec3d Q = Math::Vec3d(0.0, 0.0, 1.0);
		} orbit;
	};
}
