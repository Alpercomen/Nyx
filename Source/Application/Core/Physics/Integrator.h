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

    inline int32 FindBodyIndexByID(const Vector<SimBody>& bodies, EntityID id)
    {
        for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
        {
            if (bodies[i].id == id)
                return i;
        }

        return -1;
    }

    inline int32 ComputeTimeLevel(float64 desiredDt, float64 finestDt, int32 maxLevel)
    {
        if (desiredDt <= finestDt)
            return 0;

        int32 level = 0;
        float64 dt = finestDt;

        while (level < maxLevel && dt * 2.0 <= desiredDt)
        {
            dt *= 2.0;
            ++level;
        }

        return level;
    }

    inline float64 ComputeDesiredDt(const Vector<SimBody>& bodies, int32 selfIndex)
    {
        const SimBody& self = bodies[selfIndex];

        if (!self.active || self.mass <= 0.0)
            return DESIRED_DT_MAX;

        EntityID parentID = self.timestepParentID;
        if (parentID == NO_ID)
            parentID = self.strongestAttractorID;

        if (parentID == NO_ID)
            return DESIRED_DT_MAX;

        const int32 parentIndex = FindBodyIndexByID(bodies, parentID);
        if (parentIndex < 0)
            return DESIRED_DT_MAX;

        const SimBody& parent = bodies[parentIndex];
        if (!parent.active || parent.mass <= 0.0)
            return DESIRED_DT_MAX;

        const Math::Vec3d relPos = self.position - parent.position;
        const float64 r = glm::length(relPos);

        if (r <= 1e-6)
            return DESIRED_DT_MIN;

        const float64 mu = G * parent.mass;
        const float64 orbitalTimeScale = std::sqrt((r * r * r) / mu);

        float64 desiredDt = orbitalTimeScale * 0.02;

        desiredDt *= GLOBAL_DT_SCALE;

        if (parent.soiRadius > 1e-6)
        {
            const float64 normalizedRadius = r / parent.soiRadius;

            if (normalizedRadius < 0.1)
                desiredDt *= TIGHT_ORBIT_SCALE;
        }

        desiredDt = std::clamp(desiredDt, DESIRED_DT_MIN, DESIRED_DT_MAX);
        return desiredDt;
    }

    void AdvanceBlockTimesteps(Vector<SimBody>& bodies, IGravitySolver& solver, float64 frameDt, float64 baseDt)
    {
        if (bodies.empty() || frameDt <= 0.0)
            return;

        float64 finestDt = std::numeric_limits<float64>::infinity();

        for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
        {
            SimBody& body = bodies[i];
            body.desiredDt = ComputeDesiredDt(bodies, i);

            finestDt = std::min(finestDt, body.desiredDt);
        }

        finestDt = std::max(finestDt, 0.001);
        finestDt = std::min(finestDt, baseDt);

        const int32 maxLevel = 32;

        for (SimBody& body : bodies)
            body.timeLevel = ComputeTimeLevel(body.desiredDt, finestDt, maxLevel);

        int32 finestSteps = static_cast<int32>(std::ceil(frameDt / finestDt));
        finestSteps = std::max(1, finestSteps);

        finestDt = frameDt / static_cast<float64>(finestSteps);

        for (int32 tick = 0; tick < finestSteps; ++tick)
        {
            solver.Build(bodies);

            const float64 elapsed = tick * finestDt;
            const float64 remaining = frameDt - elapsed;

            for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
            {
                SimBody& body = bodies[i];

                if (!body.active)
                    continue;

                const int32 stride = (1 << body.timeLevel);

                if ((tick % stride) != 0)
                    continue;

                float64 bodyDt = finestDt * static_cast<float64>(stride);
                bodyDt = std::min(bodyDt, remaining);

                if (bodyDt <= 0.0)
                    continue;

                LeapfrogStepBody(bodies, i, bodyDt, solver);
            }
        }
    }

}