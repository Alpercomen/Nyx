#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Physics/Types.h>
#include <Application/Services/Solver/IGravitySolver.h>

namespace Nyx
{
    class NBodyGravitySolver final : public IGravitySolver
    {
    public:
        void Build(const Vector<SimBody>& bodies) override
        {
            // Nothing to build for direct all-pairs.
        }

        Math::Vec3d ComputeAcceleration(const Vector<SimBody>& bodies, int32 selfIndex, const Math::Vec3d& samplePosition) const override
        {
            Math::Vec3d total(0.0);

            for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
            {
                if (i == selfIndex || !bodies[i].active || bodies[i].mass <= 0.0)
                    continue;

                Math::Vec3d r = bodies[i].position - samplePosition;
                float64 distSq = glm::dot(r, r) + bodies[i].softening * bodies[i].softening;

                if (distSq < 1e-18)
                    continue;

                float64 invDist = 1.0 / std::sqrt(distSq);
                float64 invDist3 = invDist * invDist * invDist;

                total += r * (G * bodies[i].mass * invDist3);
            }

            return total;
        }


    };

}