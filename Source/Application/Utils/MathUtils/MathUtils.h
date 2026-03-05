#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Transform/Transform.h>

namespace Nyx
{
	Math::Vec3d LocalToWorld(const Math::Vec3d local, const Transform& transform);
}
