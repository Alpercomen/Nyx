#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Services/Managers/ComponentManager/ComponentManager.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Utils/QTUtils/Panel/InspectorEditors.h>
#include <Application/Resource/Components/Components.h>

void RegisterComponents();

class Editor : public Singleton<Editor>
{
public:
	Optional<EntityID> selectedEntity;
};