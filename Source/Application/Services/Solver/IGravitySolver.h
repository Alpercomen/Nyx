#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Physics/Types.h>

namespace Nyx
{
    class IGravitySolver
    {
    public:
        virtual ~IGravitySolver() = default;
        virtual void Build(const Vector<SimBody>& bodies) = 0;
        virtual Math::Vec3d ComputeAcceleration(const Vector<SimBody>& bodies, int32 selfIndex, const Math::Vec3d& samplePosition) const = 0;
    };

}