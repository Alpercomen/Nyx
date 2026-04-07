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
        Nyx::InitializeAttractors(world);
        Nyx::InitializeAccelerations(world, solver);

        const float64 frameDt = deltaTime * static_cast<float64>(SimulationControls::Get().GetTimeDesired());

        Nyx::AdvanceBlockTimesteps(world.bodies, solver, frameDt, world.baseTimeStep);
        Nyx::SyncBodiesToECS(world, frameDt);
    }
}