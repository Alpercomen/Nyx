#pragma once
#include "MathUtils.h"

namespace Nyx
{
	Math::Vec3d LocalToWorld(const Math::Vec3d local, const Transform& transform)
	{
		return transform.rotation.GetQuaternion() * local;
	}

    Math::Vec3d HermitePosition(const SourceState& a, const SourceState& b, float64 h, float64 u)
    {
        const float64 u2 = u * u;
        const float64 u3 = u2 * u;

        const float64 h00 = 2.0 * u3 - 3.0 * u2 + 1.0;
        const float64 h10 = u3 - 2.0 * u2 + u;
        const float64 h01 = -2.0 * u3 + 3.0 * u2;
        const float64 h11 = u3 - u2;

        return h00 * a.pos + (h10 * h) * a.vel + h01 * b.pos + (h11 * h) * b.vel;
    }

    Math::Vec3d AccelFromSources(const SourceTrajectory& traj, float64 t, const Math::Vec3d& p)
    {
        int32 k = static_cast<int32>(t / traj.dt);
        k = std::clamp(k, 0, traj.steps - 1);
        const float64 u = std::clamp((t - k * traj.dt) / traj.dt, 0.0, 1.0);

        Math::Vec3d a(0.0);

        for (int32 s = 0; s < traj.sourceCount; ++s)
        {
            const Math::Vec3d sp = HermitePosition(traj.At(k, s), traj.At(k + 1, s), traj.dt, u);
            const Math::Vec3d r = sp - p;
            const float64 inv = 1.0 / std::sqrt(glm::dot(r, r) + traj.eps2[s]);

            a += r * (traj.gm[s] * inv * inv * inv);
        }

        return a;
    }
}
