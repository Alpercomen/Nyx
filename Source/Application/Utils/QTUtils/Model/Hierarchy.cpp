#include <QString>

#include <Application/Utils/QTUtils/Model/Hierarchy.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>

HierarchyModel::HierarchyModel(QObject* parent) : QAbstractListModel(parent) 
{ 
    refresh();
}

int HierarchyModel::rowCount(const QModelIndex& parent) const 
{
    return parent.isValid() ? 0 : (int)m_ids.size();
}

QVariant HierarchyModel::data(const QModelIndex& index, int role) const 
{
    if (!index.isValid() || role != Qt::DisplayRole) 
        return {};

    const EntityID id = m_ids[index.row()];
    // Derive a name: Name component or "Entity <id>"
    if (ECS::Get().HasComponent<Name>(id))
    {
        auto* nameComponent = ECS::Get().GetComponent<Name>(id);
        return QString::fromUtf8(nameComponent->name.c_str());
    }
    return QString("Entity %1").arg(id);
}

void HierarchyModel::refresh()
{
    beginResetModel();

    if (!m_ids.empty())
        m_ids.clear();

    // Fill m_ids from your ECS; replace with your API
    auto all = ECS::Get().GetAllEntityIDs();
    m_ids.assign(all.begin(), all.end());
    endResetModel();
}