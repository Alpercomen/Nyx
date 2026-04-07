#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Physics/Types.h>
#include <Application/Services/Solver/IGravitySolver.h>

namespace Nyx
{
    struct Node
    {
        Math::Vec3d center = Math::Vec3d(0.0);
        float64 halfSize = 0.0;

        float64 totalMass = 0.0;
        Math::Vec3d centerOfMass = Math::Vec3d(0.0);

        int32 children[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
        int32 bodyIndex = -1;
        bool isLeaf = true;
    };

    class BarnesHutGravitySolver final : public IGravitySolver
    {
    public:
        void Build(const Vector<SimBody>& bodies) override
        {
            
        }

        Math::Vec3d ComputeAcceleration(const Vector<SimBody>& bodies, int32 selfIndex, const Math::Vec3d& samplePosition) const override
        {
            
            return Math::Vec3d(0.0);
        }

    private:
        Vector<Node> m_nodes;
        float64 m_theta = 0.5;
    };

}