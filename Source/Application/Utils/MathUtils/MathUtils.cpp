#pragma once
#include <Application/Utils/MathUtils/MathUtils.h>

namespace Nyx
{
	Math::Vec3f LocalToWorld(const Math::Vec3f local, const Transform& transform)
	{
		return transform.rotation.GetQuaternion() * local;
	}
}
