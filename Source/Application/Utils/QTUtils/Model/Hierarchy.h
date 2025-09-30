#pragma once
#include <QAbstractListModel>
#include <Application/Core/Core.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

using Nyx::EntityID;

class HierarchyModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit HierarchyModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    // Call this when scene changes
    void refresh();

    EntityID idAt(int row) const { return (row >= 0 && row < (int)m_ids.size()) ? m_ids[row] : 0; }

private:
    Vector<EntityID> m_ids;
};