#pragma once
#include <QWidget> // Base class
#include <QScrollArea>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

using Nyx::EntityID;

class InspectorPanel : public QScrollArea {
    Q_OBJECT
public:
    explicit InspectorPanel(QWidget* parent = nullptr);

public slots:
    void clearForm(QFormLayout* form);
    void setSelectedEntity(EntityID id);

private slots:
    void pullModelToUI();

private:
    QWidget* m_body = nullptr;
    QFormLayout* m_form = nullptr;
    EntityID     m_selected = 0;
    QTimer* m_timer = nullptr;
};

class ScientificDoubleSpinBox : public QDoubleSpinBox
{
public:
    using QDoubleSpinBox::QDoubleSpinBox;

protected:
    QString textFromValue(double value) const override
    {
        return QString::number(value, 'e', 6);
    }

    double valueFromText(const QString& text) const override
    {
        return text.toDouble();
    }
};