#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Transform/Transform.h>

namespace Nyx
{
	Math::Vec3f LocalToWorld(const Math::Vec3f local, const Transform& transform);
}
