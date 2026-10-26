#pragma once

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

#include "JsonAdapter.h"

namespace Nyx
{
	class Scene;

	namespace SceneLoader
	{
		HashMap<String, EntityID> Load(Scene& scene, const String& jsonPath);
	}
}