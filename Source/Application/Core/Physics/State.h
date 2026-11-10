#pragma once

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    struct SourceState
    {
        Math::Vec3d pos = Math::Vec3d(0.0);
        Math::Vec3d vel = Math::Vec3d(0.0);
    };

    struct SourceTrajectory
    {
        int32   sourceCount = 0;
        int32   steps = 0;
        float64 dt = 0.0;

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

        SourceState& At(int32 k, int32 s) { return states[static_cast<size_t>(k) * sourceCount + s]; }
        const SourceState& At(int32 k, int32 s) const { return states[static_cast<size_t>(k) * sourceCount + s]; }
    };
}