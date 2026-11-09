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

    struct SourceState
    {
        Math::Vec3d pos = Math::Vec3d(0.0);
        Math::Vec3d vel = Math::Vec3d(0.0);
    };

    struct SourceTrajectory
    {
        int32   sourceCount = 0;
        int32   steps       = 0;
        float64 dt          = 0.0;

        Vector<float64>     gm;
        Vector<float64>     eps2;
        Vector<SourceState> states;

        void Reset(int32 S, int32 N, float64 stepDt)
        {
            sourceCount = S;
            steps = N;
            dt = stepDt;
            gm.assign(S, 0.0);
            eps2.assign(S, 0.0);
            states.assign(static_cast<size_t>(N + 1) * S, SourceState{});
        }

        SourceState&       At(int32 k, int32 s)       { return states[static_cast<size_t>(k) * sourceCount + s]; }
        const SourceState& At(int32 k, int32 s) const { return states[static_cast<size_t>(k) * sourceCount + s]; }
    };

    inline Math::Vec3d HermitePosition(const SourceState& a, const SourceState& b, float64 h, float64 u)
    {
        const float64 u2 = u * u;
        const float64 u3 = u2 * u;

        const float64 h00 =  2.0 * u3 - 3.0 * u2 + 1.0;
        const float64 h10 =        u3 - 2.0 * u2 + u;
        const float64 h01 = -2.0 * u3 + 3.0 * u2;
        const float64 h11 =        u3 -       u2;

        return h00 * a.pos + (h10 * h) * a.vel + h01 * b.pos + (h11 * h) * b.vel;
    }

    inline Math::Vec3d AccelFromSources(const SourceTrajectory& traj, float64 t, const Math::Vec3d& p)
    {
        int32 k = static_cast<int32>(t / traj.dt);
        k = std::clamp(k, 0, traj.steps - 1);
        const float64 u = std::clamp((t - k * traj.dt) / traj.dt, 0.0, 1.0);

        Math::Vec3d a(0.0);

        for (int32 s = 0; s < traj.sourceCount; ++s)
        {
            const Math::Vec3d sp = HermitePosition(traj.At(k, s), traj.At(k + 1, s), traj.dt, u);
            const Math::Vec3d r  = sp - p;
            const float64 inv    = 1.0 / std::sqrt(glm::dot(r, r) + traj.eps2[s]);

            a += r * (traj.gm[s] * inv * inv * inv);
        }

        return a;
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
