#pragma once
#include <QFormLayout>

#include <Application/Core/Core.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

class QFormLoayout;
class QWidget;

namespace Nyx
{
	using BuildUIFn = function<void* (EntityID, QFormLayout*, QWidget*)>;

    struct ComponentInfo {
        String  displayName;
        BuildUIFn    buildUI;
    };

    class ComponentRegistry : public Singleton<ComponentRegistry> {
    public:
        template<typename T>
        void registerComponent(const std::string& displayName, BuildUIFn uiBuilder) {
            _components.emplace_back(ComponentInfo{ displayName, std::move(uiBuilder) });
            _byType[std::type_index(typeid(T))] = &_components.back();
        }

        const Vector<ComponentInfo>& components() const { return _components; }

    private:
        Vector<ComponentInfo> _components;
        HashMap<TypeIndex, const ComponentInfo*> _byType;
    };
}