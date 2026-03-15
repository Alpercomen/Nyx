#pragma once

#include <Application/Core/Core.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>

namespace Nyx
{
	class Editor : public Singleton<Editor>
	{
	public:
		Optional<EntityID> selectedEntity;
	};
}