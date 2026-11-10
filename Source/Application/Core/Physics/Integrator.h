#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Physics/Types.h>

#include <algorithm>
#include <cmath>

namespace Nyx
{
    inline void ClassifySources(Vector<SimBody>& bodies, Vector<int32>& sources, Vector<int32>& tests,
                                float64 promoteMass, float64 demoteMass)
    {
        sources.clear();
        tests.clear();

        for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
        {
            SimBody& b = bodies[i];

            if (!b.active)
            {
                b.isSource = false;
                continue;
            }

            if (b.mass <= 0.0)
                b.isSource = false;
            else if (b.isSource)
                b.isSource = b.mass >= demoteMass;
            else
                b.isSource = b.mass >= promoteMass;

            (b.isSource ? sources : tests).push_back(i);
        }
    }

    inline float64 IntegrateSources(Vector<SimBody>& bodies, const Vector<int32>& sources,
                                    float64 frameDt, SourceTrajectory& traj)
    {
        const int32 S = static_cast<int32>(sources.size());

        if (S == 0 || frameDt <= 0.0)
        {
            traj.Reset(0, 0, 0.0);
            return frameDt;
        }

        float64 dt = SOURCE_DT_MAX;
        for (int32 idx : sources)
            dt = std::min(dt, bodies[idx].desiredDt);
        dt = std::max(dt, static_cast<float64>(DESIRED_DT_MIN));

        int32 steps = std::max(1, static_cast<int32>(std::ceil(frameDt / dt)));
        float64 simulated = frameDt;

        if (steps > MAX_SOURCE_STEPS_PER_FRAME)
        {
            steps = MAX_SOURCE_STEPS_PER_FRAME;
            simulated = steps * dt;
        }
        else
        {
            dt = frameDt / steps;
        }

        traj.Reset(S, steps, dt);

        Vector<Math::Vec3d> pos(S), vel(S), acc(S);

        for (int32 s = 0; s < S; ++s)
        {
            const SimBody& b = bodies[sources[s]];
            pos[s] = b.position;
            vel[s] = b.velocity;
            traj.gm[s]   = G * b.mass;
            traj.eps2[s] = b.softening * b.softening;
        }

        auto computeAcc = [&]()
        {
            for (int32 i = 0; i < S; ++i)
            {
                Math::Vec3d a(0.0);

                for (int32 j = 0; j < S; ++j)
                {
                    if (i == j)
                        continue;

                    const Math::Vec3d r = pos[j] - pos[i];
                    const float64 inv   = 1.0 / std::sqrt(glm::dot(r, r) + traj.eps2[j]);

                    a += r * (traj.gm[j] * inv * inv * inv);
                }

                acc[i] = a;
            }
        };

        auto record = [&](int32 k)
        {
            for (int32 s = 0; s < S; ++s)
            {
                traj.At(k, s).pos = pos[s];
                traj.At(k, s).vel = vel[s];
            }
        };

        computeAcc();
        record(0);

        for (int32 k = 0; k < steps; ++k)
        {
            for (int32 s = 0; s < S; ++s)
            {
                vel[s] += 0.5 * acc[s] * dt;
                pos[s] += vel[s] * dt;
            }

            computeAcc();

            for (int32 s = 0; s < S; ++s)
                vel[s] += 0.5 * acc[s] * dt;

            record(k + 1);
        }

        for (int32 s = 0; s < S; ++s)
        {
            SimBody& b = bodies[sources[s]];
            b.position     = pos[s];
            b.velocity     = vel[s];
            b.acceleration = acc[s];
        }

        return simulated;
    }

    inline int32 IntegrateTestParticles(Vector<SimBody>& bodies, const Vector<int32>& tests,
                                        const SourceTrajectory& traj, float64 simulatedDt)
    {
        if (traj.sourceCount == 0 || traj.steps == 0 || simulatedDt <= 0.0)
            return 0;

        int32 limited = 0;
        const int32 count = static_cast<int32>(tests.size());

        #pragma omp parallel for schedule(dynamic, 64) reduction(+:limited)
        for (int32 n = 0; n < count; ++n)
        {
            SimBody& b = bodies[tests[n]];

            const float64 desired = std::max(b.desiredDt, static_cast<float64>(DESIRED_DT_MIN));
            int32 steps = std::max(1, static_cast<int32>(std::ceil(simulatedDt / desired)));

            if (steps > MAX_PARTICLE_STEPS_PER_FRAME)
            {
                steps = MAX_PARTICLE_STEPS_PER_FRAME;
                ++limited;
            }

            const float64 dt = simulatedDt / steps;

            Math::Vec3d pos = b.position;
            Math::Vec3d vel = b.velocity;
            Math::Vec3d acc = AccelFromSources(traj, 0.0, pos);

            for (int32 k = 0; k < steps; ++k)
            {
                vel += 0.5 * acc * dt;
                pos += vel * dt;
                acc  = AccelFromSources(traj, (k + 1) * dt, pos);
                vel += 0.5 * acc * dt;
            }

            b.position     = pos;
            b.velocity     = vel;
            b.acceleration = acc;
        }

        return limited;
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

    inline float64 ComputeDesiredDt(const Vector<SimBody>& bodies, int32 selfIndex, float64 maxDt = DESIRED_DT_MAX)
    {
        const SimBody& self = bodies[selfIndex];

        if (!self.active)
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

        desiredDt = std::clamp(desiredDt, DESIRED_DT_MIN, maxDt);
        return desiredDt;
    }

    inline float64 AdvanceTiered(Vector<SimBody>& bodies, const Vector<int32>& sources,
                                 const Vector<int32>& tests, float64 frameDt, int32* limitedOut = nullptr)
    {
        if (bodies.empty() || frameDt <= 0.0)
            return 0.0;

        for (int32 i : sources) bodies[i].desiredDt = ComputeDesiredDt(bodies, i, SOURCE_DT_MAX);
        for (int32 i : tests)   bodies[i].desiredDt = ComputeDesiredDt(bodies, i, PARTICLE_DT_MAX);

        static SourceTrajectory traj;

        const float64 simulated = IntegrateSources(bodies, sources, frameDt, traj);
        const int32   limited   = IntegrateTestParticles(bodies, tests, traj, simulated);

        if (limitedOut)
            *limitedOut = limited;

        return simulated;
    }
}
