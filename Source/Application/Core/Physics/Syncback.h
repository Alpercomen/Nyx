#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Physics/Types.h>
#include <Application/Core/Physics/AngularIntegration.h>
#include <Application/Services/Solver/IGravitySolver.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>

namespace Nyx
{
    void GatherBodiesFromECS(PhysicsWorld& world)
    {
        world.bodies.clear();

        for (EntityID id : ECS::Get().View<Transform, Rigidbody>())
        {
            Transform* tr = ECS::Get().GetComponent<Transform>(id);
            Rigidbody* rb = ECS::Get().GetComponent<Rigidbody>(id);

            if (!tr || !rb)
                continue;

            SimBody body;
            body.id = id;
            body.position = tr->position.GetWorld();
            body.velocity = rb->velocity.GetWorld();
            body.mass = rb->mass;
            body.softening = 1.0 / METERS_PER_UNIT;

            world.bodies.push_back(body);
        }
    }

    void InitializeAccelerations(PhysicsWorld& world, IGravitySolver& solver)
    {
        solver.Build(world.bodies);

        for (int32 i = 0; i < static_cast<int32>(world.bodies.size()); ++i)
            world.bodies[i].acceleration = solver.ComputeAcceleration(world.bodies, i, world.bodies[i].position);
    }

    void ComputeOrbitalParents(PhysicsWorld& world)
    {
        ComputeStrongestAttractors(world.bodies);
        ComputeSOIRadii(world.bodies);

        for (int32 i = 0; i < static_cast<int32>(world.bodies.size()); ++i)
            world.bodies[i].orbitalParentID = ComputeOrbitalParentID(world.bodies, i);
    }

    void SyncBodiesToECS(const PhysicsWorld& world, float64 angularDt)
    {
        for (const SimBody& body : world.bodies)
        {
            Transform* tr = ECS::Get().GetComponent<Transform>(body.id);
            Rigidbody* rb = ECS::Get().GetComponent<Rigidbody>(body.id);

            if (!tr || !rb)
                continue;

            tr->position.SetWorld(body.position);
            rb->velocity.SetWorld(body.velocity);
            rb->orbitalParentID = body.orbitalParentID;

            IntegrateAngularVelocity(*tr, *rb, angularDt);
            IntegrateTidallyLocked(body.id);
        }
    }
}