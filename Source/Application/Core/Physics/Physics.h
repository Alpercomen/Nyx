#pragma once
#include <iostream>

#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Core/Physics/Types.h>
#include <Application/Core/Physics/Syncback.h>
#include <Application/Core/Physics/Integrator.h>
#include <Application/Services/Solver/NBodyGravitySolver.h>

#include <chrono>

namespace Physics
{
    inline void Update(float64 deltaTime)
    {
        if (!SimulationControls::Get().GetIsPlaying())
            return;

        PhysicsWorld world;
        world.baseTimeStep = 1.0;
        Nyx::GatherBodiesFromECS(world);

        if (world.bodies.empty())
            return;

        Nyx::NBodyGravitySolver solver;
        Nyx::ClassifySources(world.bodies, world.sources, world. tests, 1e20, 5e19);
        Nyx::ComputeOrbitalParents(world);
        ComputeTimeStepParents(world.bodies);

        const float64 frameDt = deltaTime * static_cast<float64>(SimulationControls::Get().GetTimeDesired());
        int32 limited = frameDt;

        float64 simDt = Nyx::AdvanceTiered(world.bodies, world.sources, world.tests, frameDt, &limited);

        Nyx::ComputeOrbitalParents(world);
        Nyx::SyncBodiesToECS(world, simDt);
        OrbitTrailSystem::Get().Update();

    }
}