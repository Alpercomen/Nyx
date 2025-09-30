#pragma once
#include <QWidget> // Base class
#include <QScrollArea>
#include <QFormLayout>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

using Nyx::EntityID;

class InspectorPanel : public QScrollArea {
    Q_OBJECT
public:
    explicit InspectorPanel(QWidget* parent = nullptr);

public slots:
    void clearForm(QFormLayout* form);
    void setSelectedEntity(EntityID id);

private:
    QWidget* m_body = nullptr;
    QFormLayout* m_form = nullptr;
    EntityID     m_selected = 0;
};