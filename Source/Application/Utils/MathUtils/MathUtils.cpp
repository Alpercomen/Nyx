#pragma once

#include <Application/Utils/MathUtils/MathUtils.h>

Math::Vec3f LocalToWorld(const Math::Vec3f local, const Transform& transform)
{
	return transform.rotation.GetQuaternion() * local;
}
