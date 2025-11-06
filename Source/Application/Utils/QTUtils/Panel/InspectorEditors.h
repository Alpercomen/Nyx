#pragma once
#include <QFormLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QSignalBlocker>

#include <Application/Utils/QTUtils/Panel/Inspector.h>

// Store the raw address for pull-updates.
inline void setPtrProperty(QWidget* w, const char* key, const void* p) 
{
    w->setProperty(key, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(p)));
}

template<typename T>
inline T* getPtrProperty(const QObject* w, const char* key) 
{
    const auto v = w->property(key);
    if (!v.isValid()) 
        return nullptr;

    return reinterpret_cast<T*>(v.value<quintptr>());
}

/* --------- scalar editors: bind-by-reference + tag pointer --------- */

// BOOL
inline QWidget* addEditor(QFormLayout* form, const char* name, bool& value) 
{
    auto* w = new QCheckBox;
    w->setChecked(value);

    setPtrProperty(w, "nyx_addr_bool", &value);

    QObject::connect(w, &QCheckBox::toggled, [&value](bool v) { value = v; });
    form->addRow(new QLabel(name), w);
    return w;
}

// INTEGER
inline QWidget* addEditor(QFormLayout* form, const char* name, int& value, int min = -100000, int max = 100000) 
{
    auto* w = new QSpinBox;
    w->setRange(min, max);
    w->setValue(value);

    setPtrProperty(w, "nyx_addr_i32", &value);

    QObject::connect(w, qOverload<int>(&QSpinBox::valueChanged), [&value](int v) { value = v; });
    form->addRow(new QLabel(name), w);
    return w;
}

// FLOAT
inline QWidget* addEditor(QFormLayout* form, const char* name, float& value, double min = -1e9, double max = 1e9, double step = 0.1) 
{
    auto* w = new QDoubleSpinBox;
    w->setDecimals(6);
    w->setRange(min, max);
    w->setSingleStep(step);
    w->setValue(value);

    setPtrProperty(w, "nyx_addr_f32", &value);

    QObject::connect(w, qOverload<double>(&QDoubleSpinBox::valueChanged), [&value](double v) { value = float(v); });
    form->addRow(new QLabel(name), w);
    return w;
}

// DOUBLE
inline QWidget* addEditor(QFormLayout* form, const char* name, double& value, double min = -1e12, double max = 1e12, double step = 0.1) 
{
    auto* w = new QDoubleSpinBox;
    w->setDecimals(6);
    w->setRange(min, max);
    w->setSingleStep(step);
    w->setValue(value);

    setPtrProperty(w, "nyx_addr_f64", &value);

    QObject::connect(w, qOverload<double>(&QDoubleSpinBox::valueChanged), [&value](double v) { value = v; });
    form->addRow(new QLabel(name), w);
    return w;
}

// STRING
inline QWidget* addEditor(QFormLayout* form, const char* name, std::string& value) 
{
    auto* w = new QLineEdit(QString::fromUtf8(value.c_str()));

    setPtrProperty(w, "nyx_addr", &value);

    QObject::connect(w, &QLineEdit::textEdited, [&value](const QString& s) { value = s.toStdString(); });
    form->addRow(new QLabel(name), w);
    return w;
}

inline QWidget* addWeight(QFormLayout* form, const char* name, double& kgValue, double unitScaleKgPerUnit, int decimals = 4, double min = 0.0, double max = 1e30, double step = 0.1)
{
    auto* w = new ScientificDoubleSpinBox;
    w->setDecimals(decimals);
    w->setRange(min, max);
    w->setSingleStep(step);

    // Show in the chosen unit
    w->setValue(kgValue / unitScaleKgPerUnit);

    setPtrProperty(w, "nyx_addr_f64", &kgValue);
    w->setProperty("nyx_scale", unitScaleKgPerUnit);

    // When user edits, convert back to kg
    QObject::connect(w, qOverload<double>(&QDoubleSpinBox::valueChanged),
        [&kgValue, unitScaleKgPerUnit](double v) {
            kgValue = v * unitScaleKgPerUnit;
        });

    form->addRow(new QLabel(name), w);
    return w;
}

template<typename Vec3>
inline QWidget* addEditorVec3(QFormLayout* form, const char* name, Vec3& v) 
{
    QWidget* row = new QWidget;
    auto* layout = new QHBoxLayout(row); 

    layout->setContentsMargins(0, 0, 0, 0);

    auto* xs = new QDoubleSpinBox; 
    auto* ys = new QDoubleSpinBox; 
    auto* zs = new QDoubleSpinBox;

    for (auto* s : { xs,ys,zs })
    {
        s->setDecimals(4);
        s->setRange(-1e40, 1e40);
        s->setSingleStep(1.0);
    }

    xs->setValue(v.x);
    ys->setValue(v.y);
    zs->setValue(v.z);

    // Tag each widget with the address of the actual field it controls
    setPtrProperty(xs, "nyx_addr_f32", &v.x);
    setPtrProperty(ys, "nyx_addr_f32", &v.y);
    setPtrProperty(zs, "nyx_addr_f32", &v.z);

    QObject::connect(xs, qOverload<double>(&QDoubleSpinBox::valueChanged), [&v](double x) { v.x = float(x); });
    QObject::connect(ys, qOverload<double>(&QDoubleSpinBox::valueChanged), [&v](double y) { v.y = float(y); });
    QObject::connect(zs, qOverload<double>(&QDoubleSpinBox::valueChanged), [&v](double z) { v.z = float(z); });

    layout->addWidget(xs); layout->addWidget(ys); layout->addWidget(zs);
    form->addRow(new QLabel(name), row);
    return row;
}