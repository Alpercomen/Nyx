#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Transform/Transform.h>
#include <Application/Core/Physics/State.h>

namespace Nyx
{
	Math::Vec3d LocalToWorld(const Math::Vec3d local, const Transform& transform);

    Math::Vec3d HermitePosition(const SourceState& a, const SourceState& b, float64 h, float64 u);

    Math::Vec3d AccelFromSources(const SourceTrajectory& traj, float64 t, const Math::Vec3d& p);

}
