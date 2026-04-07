#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Physics/Types.h>
#include <Application/Services/Solver/IGravitySolver.h>

namespace Nyx
{
    inline void LeapfrogStepBody(Vector<SimBody>& bodies, int32 bodyIndex, float64 dt, const IGravitySolver& solver)
    {
        SimBody& body = bodies[bodyIndex];

        body.velocity += 0.5 * body.acceleration * dt;
        body.position += body.velocity * dt;
        body.acceleration = solver.ComputeAcceleration(bodies, bodyIndex, body.position);
        body.velocity += 0.5 * body.acceleration * dt;
    }

    inline int32 ChooseTimeLevel(const SimBody& body, float64 baseDt)
    {
        const float64 speed = glm::length(body.velocity);
        const float64 accel = glm::length(body.acceleration);

        if (accel < 1e-12)
            return 0;

        const float64 characteristicTime = std::max(1e-3, std::max(speed, 1.0) / accel);
        const float64 desiredDt = characteristicTime * 0.02;

        int32 level = 0;
        float64 dt = baseDt;

        while (dt > desiredDt && level < 12)
        {
            dt *= 0.5;
            ++level;
        }

        return level;
    }

    void AdvanceLevel(Vector<SimBody>& bodies, IGravitySolver& solver, float64 dt, int32 currentLevel, int32 maxLevel)
    {
        if (currentLevel == maxLevel)
        {
            solver.Build(bodies);

            for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
            {
                if (!bodies[i].active)
                    continue;

                if (bodies[i].timeLevel >= currentLevel)
                    LeapfrogStepBody(bodies, i, dt, solver);
            }

            return;
        }

        AdvanceLevel(bodies, solver, dt * 0.5, currentLevel + 1, maxLevel);
        AdvanceLevel(bodies, solver, dt * 0.5, currentLevel + 1, maxLevel);

        solver.Build(bodies);

        for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
        {
            if (!bodies[i].active)
                continue;

            if (bodies[i].timeLevel == currentLevel)
                LeapfrogStepBody(bodies, i, dt, solver);
        }
    }

    void AdvanceBlockTimesteps(Vector<SimBody>& bodies, IGravitySolver& solver, float64 frameDt, float64 baseDt)
    {
        int32 frameSubsteps = static_cast<int32>(std::ceil(frameDt / baseDt));
        frameSubsteps = std::max(1, frameSubsteps);

        float64 actualBaseDt = frameDt / static_cast<float64>(frameSubsteps);

        for (int32 step = 0; step < frameSubsteps; ++step)
        {
            solver.Build(bodies);

            for (SimBody& body : bodies)
                body.timeLevel = ChooseTimeLevel(body, actualBaseDt);

            int32 maxLevel = 0;
            for (const SimBody& body : bodies)
                maxLevel = std::max(maxLevel, body.timeLevel);

            AdvanceLevel(bodies, solver, actualBaseDt, 0, maxLevel);
        }
    }

}