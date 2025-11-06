#pragma once
#include <Application/Core/Services/Editor/EditorComponentUI.h>
#include <Application/Core/Services/Managers/EffectManager/Atmosphere/AtmosphereSystem.h>

void RegisterComponents()
{
    // Transform
    ComponentRegistry::Get().registerComponent<Transform>("Transform",
        [](EntityID id, QFormLayout* form, QWidget*) -> void* {
            if (!ECS::Get().HasComponent<Transform>(id)) 
                return nullptr;

            auto& t = *ECS::Get().GetComponent<Transform>(id);

            addEditorVec3(form, "Position", t.position.GetWorld());
            addEditorVec3(form, "Rotation", t.rotation.GetQuaternion());
            addEditorVec3(form, "Scale", t.scale.get());
            return &t;
        });

    ComponentRegistry::Get().registerComponent<Camera>("Camera",
        [](EntityID id, QFormLayout* form, QWidget*) -> void* {
            if (!ECS::Get().HasComponent<Camera>(id)) 
                return nullptr;

            auto& c = *ECS::Get().GetComponent<Camera>(id);

            addEditor(form, "FOV", c.GetCameraDesc().Zoom, 1.0, 179.0, 0.1);
            addEditor(form, "Near", c.GetCameraDesc().NearPlane, 0.001, 1000.0, 0.001);
            addEditor(form, "Far", c.GetCameraDesc().FarPlane, 1.0, 1000000.0, 1.0);

            addEditor(form, "Yaw", c.GetCameraDesc().Yaw, -360.0, 360.0, 0.1);
            addEditor(form, "Pitch", c.GetCameraDesc().Pitch, -90.0, 90.0, 0.1);

            addEditorVec3(form, "Position", c.GetPosition().GetWorld());
            return &c;
        });

    ComponentRegistry::Get().registerComponent<Rigidbody>("Rigidbody",
        [](EntityID id, QFormLayout* form, QWidget*) -> void* {
            if (!ECS::Get().HasComponent<Rigidbody>(id))
                return nullptr;

            auto& r = *ECS::Get().GetComponent<Rigidbody>(id);

            addWeight(form, "Mass (Mt)", r.mass, KILOGRAM_PER_MEGATON, 4);
            addWeight(form, "Mass (M☉)", r.mass, SUN_MASS, 12);

            addEditorVec3(form, "Acceleration", r.acceleration.GetWorld());
            addEditorVec3(form, "Angular Velocity", r.angularVelocity.GetWorld());
            addEditorVec3(form, "Velocity", r.velocity.GetWorld());

            return &r;
        });

    ComponentRegistry::Get().registerComponent<TidallyLocked>("Tidal Lock",
        [](EntityID id, QFormLayout* form, QWidget*) -> void* {
            if (!ECS::Get().HasComponent<TidallyLocked>(id))
                return nullptr;

            auto& l = *ECS::Get().GetComponent<TidallyLocked>(id);

            Name& name = *ECS::Get().GetComponent<Name>(l.lockedEntity);

            addEditor(form, "Locked Entity", name.name);
            return &l;
        });

    ComponentRegistry::Get().registerComponent<Atmosphere>("Atmosphere",
        [](EntityID id, QFormLayout* form, QWidget*) -> void* {
            if (!ECS::Get().HasComponent<Atmosphere>(id))
                return nullptr;

            auto& a = *ECS::Get().GetComponent<Atmosphere>(id);

            AtmosphereDesc& atmosphereDesc = a.m_atmosphereDesc;

            addEditorVec3(form, "Scatter Color", atmosphereDesc.scatteringColor);
            addEditor(form, "Intensity", atmosphereDesc.intensity);
            addEditor(form, "Thickness", atmosphereDesc.atmosphereThickness);

            return &a;
        });
}
