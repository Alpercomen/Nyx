#pragma once
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSignalBlocker>

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

    m_form->addRow(new QLabel("No selection"));

    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    connect(m_timer, &QTimer::timeout, this, &InspectorPanel::pullModelToUI);
    m_timer->start();
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

    if (id == 0) {
        m_form->addRow(new QLabel("No selection"));
        return;
    }

    for (const auto& info : ComponentRegistry::Get().components())
    {
        QWidget* group = new QWidget;
        auto* sub = new QFormLayout(group); sub->setContentsMargins(0, 0, 0, 0);

        void* compPtr = info.buildUI(id, sub, group);
        if (compPtr) {
            auto* hdr = new QLabel(QString::fromUtf8(info.displayName.c_str()));
            QFont f = hdr->font(); f.setBold(true); hdr->setFont(f);
            m_form->addRow(hdr);
            m_form->addRow(group);
        }
        else {
            group->deleteLater();
        }
    }

    if (m_form->rowCount() == 0)
        m_form->addRow(new QLabel("No components"));

    // one immediate pull so it reflects latest right away
    pullModelToUI();
}

void InspectorPanel::pullModelToUI()
{
    if (!m_body) return;

    const auto widgets = m_body->findChildren<QWidget*>();
    for (QWidget* w : widgets)
    {
        // ----- QDoubleSpinBox (double/float-backed) -----
        if (auto* ds = qobject_cast<QDoubleSpinBox*>(w))
        {
            // Prefer double binding
            if (auto* pd = getPtrProperty<double>(ds, "nyx_addr_f64"))
            {
                double v = *pd; // stored in kg or raw units
                if (ds->property("nyx_scale").isValid()) {
                    const double s = ds->property("nyx_scale").toDouble();
                    if (s != 0.0) v /= s;  // kg -> display unit
                }
                if (ds->value() != v) { QSignalBlocker b(ds); ds->setValue(v); }
                continue;
            }
            // Then float binding
            if (auto* pf = getPtrProperty<float>(ds, "nyx_addr_f32"))
            {
                double v = static_cast<double>(*pf);
                if (ds->property("nyx_scale").isValid()) {
                    const double s = ds->property("nyx_scale").toDouble();
                    if (s != 0.0) v /= s;
                }
                if (ds->value() != v) { QSignalBlocker b(ds); ds->setValue(v); }
                continue;
            }
        }

        // ----- QSpinBox (int) -----
        if (auto* isb = qobject_cast<QSpinBox*>(w))
        {
            if (auto* pi = getPtrProperty<int>(isb, "nyx_addr_i32"))
            {
                if (isb->value() != *pi) { QSignalBlocker b(isb); isb->setValue(*pi); }
                continue;
            }
        }

        // ----- QCheckBox (bool) -----
        if (auto* cb = qobject_cast<QCheckBox*>(w))
        {
            if (auto* pb = getPtrProperty<bool>(cb, "nyx_addr_bool"))
            {
                if (cb->isChecked() != *pb) { QSignalBlocker b(cb); cb->setChecked(*pb); }
                continue;
            }
        }

        // ----- QLineEdit (std::string) -----
        if (auto* le = qobject_cast<QLineEdit*>(w))
        {
            if (auto* ps = getPtrProperty<std::string>(le, "nyx_addr_str"))
            {
                const QString v = QString::fromUtf8(ps->c_str());
                if (le->text() != v) { QSignalBlocker b(le); le->setText(v); }
                continue;
            }
        }
    }
}