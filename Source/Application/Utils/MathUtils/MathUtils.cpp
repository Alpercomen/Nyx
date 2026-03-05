#pragma once
#include <Application/Utils/MathUtils/MathUtils.h>

namespace Nyx
{
	Math::Vec3d LocalToWorld(const Math::Vec3d local, const Transform& transform)
	{
		return transform.rotation.GetQuaternion() * local;
	}
}
