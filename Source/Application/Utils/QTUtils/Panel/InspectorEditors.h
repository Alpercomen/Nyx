#pragma once
#include <QFormLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QWidget>

inline QWidget* addEditor(QFormLayout* form, const char* name, bool value)
{
    auto* w = new QCheckBox;

    w->setChecked(value);

    QObject::connect(w, &QCheckBox::toggled, [&value](bool v) { value = v; });
    form->addRow(new QLabel(name), w);

    return w;
}

inline QWidget* addEditor(QFormLayout* form, const char* name, int value, int min = -100000, int max = 100000) 
{
    auto* w = new QSpinBox;

    w->setRange(min, max);
    w->setValue(value);

    QObject::connect(w, qOverload<int>(&QSpinBox::valueChanged), [&value](int v) { value = v; });
    form->addRow(new QLabel(name), w);

    return w;
}

inline QWidget* addEditor(QFormLayout* form, const char* name, float value, double min = -1e9, double max = 1e9, double step = 0.1) 
{
    auto* w = new QDoubleSpinBox;

    w->setDecimals(4);
    w->setRange(min, max);
    w->setSingleStep(step);
    w->setValue(value);

    QObject::connect(w, qOverload<double>(&QDoubleSpinBox::valueChanged), [&value](double v) { value = float(v); });
    form->addRow(new QLabel(name), w);

    return w;
}

inline QWidget* addEditor(QFormLayout* form, const char* name, double value, double min = -1e12, double max = 1e12, double step = 0.1) 
{
    auto* w = new QDoubleSpinBox;

    w->setDecimals(6);
    w->setRange(min, max);
    w->setSingleStep(step);
    w->setValue(value);

    QObject::connect(w, qOverload<double>(&QDoubleSpinBox::valueChanged), [&value](double v) { value = v; });
    form->addRow(new QLabel(name), w);

    return w;
}

inline QWidget* addEditor(QFormLayout* form, const char* name, String value) 
{
    auto* w = new QLineEdit(QString::fromUtf8(value.c_str()));
    QObject::connect(w, &QLineEdit::textEdited, [&value](const QString& s) { value = s.toStdString(); });
    form->addRow(new QLabel(name), w);

    return w;
}

template<typename Vec3>
inline QWidget* addEditorVec3(QFormLayout* form, const char* name, Vec3 v)
{
    QWidget* row = new QWidget;

    auto* layout = new QHBoxLayout(row); layout->setContentsMargins(0, 0, 0, 0);
    auto* xs = new QDoubleSpinBox; auto* ys = new QDoubleSpinBox; auto* zs = new QDoubleSpinBox;

    for (auto* s : { xs,ys,zs }) 
    { 
        s->setDecimals(4);
        s->setRange(-1e40, 1e40);
        s->setSingleStep(0.1);
    }

    xs->setValue(v.x);
    ys->setValue(v.y);
    zs->setValue(v.z);

    QObject::connect(xs, qOverload<double>(&QDoubleSpinBox::valueChanged), [&v](double x) { v.x = float(x); });
    QObject::connect(ys, qOverload<double>(&QDoubleSpinBox::valueChanged), [&v](double y) { v.y = float(y); });
    QObject::connect(zs, qOverload<double>(&QDoubleSpinBox::valueChanged), [&v](double z) { v.z = float(z); });

    layout->addWidget(xs); layout->addWidget(ys); layout->addWidget(zs);

    form->addRow(new QLabel(name), row);

    return row;
}