#pragma once
#include <QVBoxLayout>
#include <QLabel>

#include <Application/Utils/QTUtils/Panel/Inspector.h>
#include <Application/Utils/QTUtils/Panel/InspectorEditors.h>
#include <Application/Core/Services/Managers/ComponentManager/ComponentManager.h>

using Nyx::EntityID;
using Nyx::ComponentRegistry;

InspectorPanel::InspectorPanel(QWidget* parent) : QScrollArea(parent)
{
    setWidgetResizable(true);
    m_body = new QWidget;
    m_form = new QFormLayout(m_body);
    m_form->setLabelAlignment(Qt::AlignLeft);
    m_form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    setWidget(m_body);

    // empty state
    m_form->addRow(new QLabel("No selection"));
}

void InspectorPanel::clearForm(QFormLayout* form)
{
    // walk backwards so indexes stay valid
    for (int i = form->count() - 1; i >= 0; --i) {
        if (QLayoutItem* it = form->takeAt(i)) {
            if (auto* w = it->widget()) w->deleteLater();
            if (auto* lay = it->layout()) delete lay;
            delete it;
        }
    }
}

void InspectorPanel::setSelectedEntity(EntityID id)
{
    m_selected = id;

    clearForm(m_form);

    if (id == 0)
    {
        m_form->addRow(new QLabel("No selection"));
        return;
    }

    // Loop all registered components; each builder:
    //  - checks if the entity has the component
    //  - if present, adds UI and returns the component pointer
    for (const auto& info : ComponentRegistry::Get().components())
    {
        QWidget* group = new QWidget; // each component as a subform
        auto* sub = new QFormLayout(group); 
        sub->setContentsMargins(0, 0, 0, 0);

        void* compPtr = info.buildUI(id, sub, group);
        if (compPtr)
        {
            // Add a bold label as section header
            auto* hdr = new QLabel(QString::fromUtf8(info.displayName.c_str()));
            QFont f = hdr->font(); f.setBold(true); hdr->setFont(f);
            m_form->addRow(hdr);
            m_form->addRow(group);
        }
        else 
        {
            group->deleteLater();
        }
    }

    if (m_form->rowCount() == 0)
        m_form->addRow(new QLabel("No components"));
}