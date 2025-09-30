#pragma once
#include <Application/Core/Services/Editor/EditorComponentUI.h>

void RegisterComponents()
{
    // Transform
    ComponentRegistry::Get().registerComponent<Transform>("Transform",
        [](EntityID id, QFormLayout* form, QWidget*) -> void* {
            if (!ECS::Get().HasComponent<Transform>(id))
                return nullptr;

            auto& t = *ECS::Get().GetComponent<Transform>(id);

            Position scaledPos = t.position / METERS_PER_UNIT;
            Scale scaledSca = t.scale / METERS_PER_UNIT;
            

            addEditorVec3(form, "Position", scaledPos.GetWorld());
            addEditorVec3(form, "Rotation", t.rotation.GetEulerAngles());
            addEditorVec3(form, "Scale", scaledSca.get());
            return &t;
        });

    // Camera
    ComponentRegistry::Get().registerComponent<Camera>("Camera",
        [](EntityID id, QFormLayout* form, QWidget*) -> void* {
            if (!ECS::Get().HasComponent<Camera>(id))
                return nullptr;

            auto& c = *ECS::Get().GetComponent<Camera>(id);
            addEditor(form, "FOV", c.GetZoom(), 1.0, 179.0, 0.1);
            addEditor(form, "Near", c.GetNearPlane(), 0.001, 1000.0, 0.001);
            addEditor(form, "Far", c.GetFarPlane(), 1.0, 100000.0, 1.0);
            return &c;
        });
}
