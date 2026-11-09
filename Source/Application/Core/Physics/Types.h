#pragma once

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    enum class ReferenceFrame
    {
        Inertial = 0,
        AttractorEquatorial = 1
    };

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
        bool8 isSource = false;

        float64 desiredDt = 0.0;
    };

    struct PhysicsWorld
    {
        Vector<SimBody> bodies;
        Vector<int32> sources; // Holds the index of large objects = Sun, Jupiter, Saturn etc.
        Vector<int32> tests; // Holds the index of smaller objects = ISS, Hubble Space Telescope etc.

        float64 baseTimeStep = 1.0;
        float64 currentTime = 0.0;
    };

    struct OrbitalElements
    {
        bool8 valid = false;

		float64 semiMajorAxis = 0.0;
        float64 semiMinorAxis = 0.0;
		float64 eccentricity = 0.0;
		float64 inclination = 0.0;
		float64 longitudeAscendingNode = 0.0;
		float64 argumentOfPeriapsis = 0.0;
		float64 trueAnomaly = 0.0;

        Math::Vec3d P = Math::Vec3d(1.0, 0.0, 0.0);
        Math::Vec3d Q = Math::Vec3d(0.0, 0.0, 1.0);

        ReferenceFrame referenceFrame = ReferenceFrame::Inertial;
    };
}