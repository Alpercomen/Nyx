#pragma once

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    struct SimBody
    {
        EntityID id = NO_ID;
        EntityID orbitalParentID = NO_ID;
        EntityID strongestAttractorID = NO_ID;
        EntityID timestepParentID = NO_ID;

        Math::Vec3d position;
        Math::Vec3d rotation;
        Math::Vec3d velocity;
        Math::Vec3d acceleration;
        float64 mass;
        float64 soiRadius = 0.0;

        float64 softening = 0.0;
        int32 timeLevel = 0;
        bool8 active = true;

        float64 desiredDt = 0.0;
    };

    struct PhysicsWorld
    {
        Vector<SimBody> bodies;
        float64 baseTimeStep = 1.0;
        float64 currentTime = 0.0;
    };

    struct OrbitalElements
    {
		float64 semiMajorAxis = 0.0;
		float64 eccentricity = 0.0;
		float64 inclination = 0.0;
		float64 longitudeAscendingNode = 0.0;
		float64 argumentOfPeriapsis = 0.0;
		float64 trueAnomaly = 0.0;
    };
}