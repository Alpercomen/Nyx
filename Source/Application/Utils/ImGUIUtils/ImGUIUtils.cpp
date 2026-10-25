#include <vector>
#include <iostream>

#include <spdlog/spdlog.h>
#include <Application/Utils/ImGUIUtils/ImGUIUtils.h>
#include <Application/Services/Editor/Editor.h>
#include <Application/Services/Camera/CameraService.h>
#include <Application/Services/SimulationControl/SimulationControl.h>

void ImGUIUtils::Initialize(void* window)
{
    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    spdlog::info("ImGui Version: {:30}", IMGUI_VERSION);

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGUIUtils::InitDockableWindow()
{
    // If docking is enabled, set dockspace.
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGuiID dockspace_id = ImGui::GetID("MainDockspace");

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // Optional flags:
        // dockspace_flags |= ImGuiDockNodeFlags_NoSplit;
        // dockspace_flags |= ImGuiDockNodeFlags_NoResize;
        // dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode;
        // dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
        // dockspace_flags |= ImGuiDockNodeFlags_AutoHideTabBar;

        ImGui::DockSpaceOverViewport(dockspace_id, viewport, dockspace_flags);
    }
}

ImVec2 ImGUIUtils::DrawGameWindow(Engine* engine)
{
    GLuint sceneTextureID = engine->GetSceneColorTex();

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;

    ImGui::Begin("Game View");
    ImVec2 textureSize = ImGui::GetContentRegionAvail();
    ImVec2 gameViewPos = ImGui::GetWindowPos();
    ImVec2 gameViewSize = ImGui::GetWindowSize();
    ImGui::Image(
        (ImTextureID)(intptr_t)sceneTextureID,
        textureSize,
        ImVec2(0, 1),  // uv0
        ImVec2(1, 0),  // uv1
        ImVec4(1, 1, 1, 1),  // tint (white)
        ImVec4(0, 0, 0, 0)   // border (none)
    );
    ImGui::End();

    float padding = 50.0f;
    float checkboxWidth = 140.0f;

    ImVec2 overlayPos(
        gameViewPos.x + gameViewSize.x - 140.0f - padding,
        gameViewPos.y + padding
    );

    ImGui::SetNextWindowPos(overlayPos, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGui::Begin("GameViewOverlay", nullptr, flags);
    ImGui::Checkbox("Lock Orientation", &CameraService::Get().lockOrientation);
    ImGui::Checkbox("Follow Target", &CameraService::Get().enabled);
    ImGui::Checkbox("Show Grid", &engine->GetRenderer().m_gridEnabled);
    ImGui::End();

    return textureSize;
}

void ImGUIUtils::DrawSimulationControl(Engine* engine)
{
    int32 desiredTime = SimulationControls::Get().GetTimeDesired();

    ImGui::Begin("Simulation Control");

    if (ImGui::Button("Play"))
        SimulationControls::Get().SetIsPlaying(true);

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
        SimulationControls::Get().SetIsPlaying(false);

    ImGui::SameLine();

    if (ImGui::Button("Fast Forward x10"))
        SimulationControls::Get().SetTimeDesired(desiredTime * 10);

    ImGui::SameLine();

    if (ImGui::Button("Fast Backward /10"))
        SimulationControls::Get().SetTimeDesired(desiredTime / 10);

    ImGui::Text("Playing: %s", SimulationControls::Get().GetIsPlaying() ? "Yes" : "No");
    ImGui::Text("Desired Speed: %i", desiredTime);
    ImGui::End();
}

void ImGUIUtils::DrawHierarchy()
{
    Optional<EntityID>& selectedEntity = Editor::Get().selectedEntity;

    ImGui::Begin("Hierarchy");
    for (EntityID entityID : ECS::Get().View<Name>())
    {
        Name& name = *ECS::Get().GetComponent<Name>(entityID);
        bool selected = (selectedEntity.has_value() && selectedEntity.value() == entityID);
        if (ImGui::Selectable(name.name.data(), selected))
            selectedEntity = entityID;
    }
    ImGui::End();
}

void ImGUIUtils::DrawName(EntityID& id)
{
    if (ECS::Get().HasComponent<Name>(id))
    {
        String& name = ECS::Get().GetComponent<Name>(id)->name;

        ImGui::Text("[%s]", name.data());
    }
}

void ImGUIUtils::DrawTransform(EntityID& id)
{
    if (ECS::Get().HasComponent<Transform>(id))
    {
        ImGui::Separator();
        ImGui::Text("[Transform]");
        auto& transform = *ECS::Get().GetComponent<Transform>(id);

        const auto& pos = transform.position.GetWorld();
        const auto& rot = transform.rotation.GetEulerAngles();
        const auto& sca = transform.scale.get();

        static double posVec[3];

        posVec[0] = pos.x;
        posVec[1] = pos.y;
        posVec[2] = pos.z;

        static double rotVec[3];

        rotVec[0] = rot.x;
        rotVec[1] = rot.y;
        rotVec[2] = rot.z;

        static double scaVec[3];

        scaVec[0] = sca.x;
        scaVec[1] = sca.y;
        scaVec[2] = sca.z;

        bool hasCamera = ECS::Get().HasComponent<Camera>(id);

        if (hasCamera)
        {
            const auto& camera = *ECS::Get().GetComponent<Camera>(id);
            ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
            ImGui::Text("Yaw: %.2f", camera.GetYaw());
            ImGui::Text("Pitch: %.2f", camera.GetPitch());
        }
        else
        {
            DragVec3Colored("Position:", "pos", posVec, 10000000.0f, "km", glm::length(pos));
            DragVec3Colored("Rotation:", "rot", rotVec, 10.0f, "rad", glm::length(rot));
            DragVec3Colored("Scale:", "sca", scaVec, 10000.0f, "km", glm::length(sca), "%.10f");
        }

        transform.position.SetWorld(Math::Vec3d{ posVec[0], posVec[1], posVec[2]});
        transform.rotation.SetFromEuler(rotVec[0], rotVec[1], rotVec[2]);
        transform.scale.set(scaVec[0], scaVec[1], scaVec[2]);
    }
}

void ImGUIUtils::DrawRigidbody(EntityID& id)
{
    if (ECS::Get().HasComponent<Rigidbody>(id))
    {
        ImGui::Separator();
        ImGui::Text("[Rigidbody]");
        auto& rigidbody = *ECS::Get().GetComponent<Rigidbody>(id);

        const auto& vel = rigidbody.velocity.GetWorld();
        const auto& acc = rigidbody.acceleration.GetWorld();
        const auto& angularVel = rigidbody.angularVelocity.GetWorld();

        static double velVec[3];

        velVec[0] = vel.x;
        velVec[1] = vel.y;
        velVec[2] = vel.z;

        static double accVec[3];

        accVec[0] = acc.x;
        accVec[1] = acc.y;
        accVec[2] = acc.z;

        static double angVec[3];

        angVec[0] = angularVel.x;
        angVec[1] = angularVel.y;
        angVec[2] = angularVel.z;

        DragVec3WithMagnitude("Velocity:", "vel", velVec, 1000.0f, "km/s", glm::length(vel));
        DragVec3WithMagnitude("Acceleration:", "acc", accVec, 100.0f, "km/s2", glm::length(acc));
        DragVec3WithMagnitude("Angular Velocity:", "angvel", angVec, 0.000001f, "km/s", glm::length(angularVel), "%.10f");

        rigidbody.velocity.SetWorld(Math::Vec3d{ velVec[0], velVec[1], velVec[2] });
        rigidbody.acceleration.SetWorld(Math::Vec3d{ accVec[0], accVec[1], accVec[2] });
        rigidbody.angularVelocity.SetWorld(Math::Vec3d{ angVec[0], angVec[1], angVec[2] });
    }
}

void ImGUIUtils::DrawAtmosphereComponent(EntityID& id)
{
    if (ECS::Get().HasComponent<AtmosphereComponent>(id))
    {
        ImGui::Separator();
        ImGui::Text("[Atmosphere]");
        auto& atmosphere = *ECS::Get().GetComponent<AtmosphereComponent>(id);

        ImGui::PushID(&atmosphere);

        float32 color[3] = { atmosphere.color.x, atmosphere.color.y, atmosphere.color.z };

        if (ImGui::ColorPicker3("Color", color,
            ImGuiColorEditFlags_DisplayRGB |
            ImGuiColorEditFlags_Float |
            ImGuiColorEditFlags_HDR))
        {
            atmosphere.color.x = color[0];
            atmosphere.color.y = color[1];
            atmosphere.color.z = color[2];
        }

        ImGui::DragFloat("Radius", &atmosphere.radius, 0.0005f, 0.0f, 100.0f, "%.3f");
        ImGui::DragFloat("Halo Intensity", &atmosphere.haloIntensity, 0.0005f, 0.0f, 100.0f, "%.3f");
        ImGui::DragFloat("Halo Alpha", &atmosphere.haloAlpha, 0.0005f, 0.0f, 100.0f, "%.3f");
        ImGui::DragFloat("Rim Start", &atmosphere.rimStart, 0.0005f, 0.0f, 100.0f, "%.3f");
        ImGui::DragFloat("Rim End", &atmosphere.rimEnd, 0.0005f, 0.0f, 100.0f, "%.3f");
        ImGui::DragFloat("Light Softness", &atmosphere.lightSoftness, 0.0005f, 0.0f, 100.0f, "%.3f");

        ImGui::PopID();
    }
}

void ImGUIUtils::DrawOrbitalParameters(EntityID& id)
{
    if (ECS::Get().HasComponent<Rigidbody>(id) && ECS::Get().HasComponent<Transform>(id))
    {
        ImGui::Separator();
        ImGui::Text("[Orbital Parameters]");
        const auto& rigidbody = *ECS::Get().GetComponent<Rigidbody>(id);
        const auto& transform = *ECS::Get().GetComponent<Transform>(id);

        const auto& orbitalParentID = rigidbody.orbitalParentID;

        if (!ECS::Get().HasComponent<Rigidbody>(orbitalParentID) || !ECS::Get().HasComponent<Transform>(orbitalParentID))
            return;

        const auto& attractorTransform = *ECS::Get().GetComponent<Transform>(orbitalParentID);
        const auto& attractorRigidbody = *ECS::Get().GetComponent<Rigidbody>(orbitalParentID);

        const auto& relPos = attractorTransform.position.GetWorld() - transform.position.GetWorld();
        const auto& relVel = attractorRigidbody.velocity.GetWorld() - rigidbody.velocity.GetWorld();
        const auto& relAcc = attractorRigidbody.acceleration.GetWorld() - rigidbody.acceleration.GetWorld();


        if (ECS::Get().HasComponent<Name>(orbitalParentID))
        {
            String& name = ECS::Get().GetComponent<Name>(orbitalParentID)->name;

            ImGui::Text("Orbitting: [%s]", name.data());
        }

        ImGui::Text("Relative Pos: (%.2f, %.2f, %.2f)", relPos.x, relPos.y, relPos.z);
        ImGui::Text("Relative Dist: %.2f km", glm::length(relPos) / METER_PER_KILOMETER);
        ImGui::Text("Distance to surface: %.2f km", (glm::length(relPos) - attractorTransform.scale.get().x) / METER_PER_KILOMETER);
        ImGui::Text("Relative Vel: %.2f km/s", glm::length(relVel));
        ImGui::Text("Relative Acc: %.2f km/s2", glm::length(relAcc));
    }
}

void ImGUIUtils::DrawOrbitTrail(EntityID& id)
{
    if (ECS::Get().HasComponent<OrbitTrail>(id))
    {
        ImGui::Separator();
        ImGui::Text("[Orbit]");

        OrbitTrail& trail = *ECS::Get().GetComponent<OrbitTrail>(id);
        ImGui::Checkbox("Show Orbit", &trail.enabled);

        ImGui::DragFloat("Fade Power", &trail.fadePower);
        ImGui::DragFloat("Opacity", &trail.opacity);

        ImGui::PushID(&trail);

        float32 color[3] = { trail.color.x, trail.color.y, trail.color.z };

        if (ImGui::ColorPicker3("Color", color,
            ImGuiColorEditFlags_DisplayRGB |
            ImGuiColorEditFlags_Float |
            ImGuiColorEditFlags_HDR))
        {
            trail.color.x = color[0];
            trail.color.y = color[1];
            trail.color.z = color[2];
        }

        ImGui::PopID();
    }
}

void ImGUIUtils::DragVec3WithMagnitude(const char* label, const char* id, float64* v, float32 speed, const char* unit, float64 magnitude, const char* fmt)
{
    ImGui::PushID(id);
    ImGui::TextUnformatted(label);
    ImGui::SameLine();

    char buf[64];
    snprintf(buf, sizeof(buf), "%.2f %s", magnitude, unit);
    float reserve = ImGui::CalcTextSize(buf).x + ImGui::GetStyle().ItemSpacing.x;

    ImGui::SetNextItemWidth(-reserve);
    ImGui::DragScalarN("##v", ImGuiDataType_Double, v, 3, speed, NULL, NULL, fmt);
    ImGui::SameLine();
    ImGui::TextUnformatted(buf);
    ImGui::PopID();
}

void ImGUIUtils::DragVec3Colored(const char* label, const char* id, float64* v, float32 speed, const char* unit, float64 magnitude, const char* fmt)
{
    static const ImU32 axisColors[3] = {
        IM_COL32(224, 82, 82, 255),   // X - red
        IM_COL32(94, 190, 94, 255),   // Y - green
        IM_COL32(82, 136, 224, 255),  // Z - blue
    };
    const float barWidth = 3.0f;

    ImGui::PushID(id);
    ImGui::TextUnformatted(label);
    ImGui::SameLine();

    char buf[64];
    snprintf(buf, sizeof(buf), "%.2f %s", magnitude, unit);
    float reserve = ImGui::CalcTextSize(buf).x + ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetNextItemWidth(-reserve);

    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

    ImDrawList* dl = ImGui::GetWindowDrawList();
    for (int i = 0; i < 3; i++)
    {
        ImGui::PushID(i);
        if (i > 0)
            ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

        ImGui::DragScalar("##c", ImGuiDataType_Double, &v[i], speed, NULL, NULL, fmt);

        ImVec2 mn = ImGui::GetItemRectMin();
        ImVec2 mx = ImGui::GetItemRectMax();
        float rounding = ImGui::GetStyle().FrameRounding;
        dl->AddRectFilled(mn, ImVec2(mn.x + barWidth, mx.y), axisColors[i], rounding, ImDrawFlags_RoundCornersLeft);

        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::TextUnformatted(buf);
    ImGui::PopID();
}

void ImGUIUtils::DrawInspector()
{
    Optional<EntityID>& selectedEntity = Editor::Get().selectedEntity;

    ImGui::Begin("Inspector");
    if (selectedEntity.has_value())
    {
        EntityID& id = selectedEntity.value();
        
        DrawName(id);
        DrawTransform(id);
        DrawRigidbody(id);
        DrawOrbitalParameters(id);
        DrawAtmosphereComponent(id);
        DrawOrbitTrail(id);
    }
    ImGui::End();
}

void ImGUIUtils::DrawWindow(Engine* enginePtr, Scene* scenePtr)
{
    Optional<EntityID>& selectedEntity = Editor::Get().selectedEntity;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGUIUtils::InitDockableWindow();
    ImVec2 textureSize = ImGUIUtils::DrawGameWindow(enginePtr);
    ImGUIUtils::DrawSimulationControl(enginePtr);
    ImGUIUtils::DrawHierarchy();
    ImGUIUtils::DrawInspector();

    Math::Vec2f textureSizeVec = { (int)textureSize.x, (int)textureSize.y };
    enginePtr->ResizeFBO(textureSizeVec, scenePtr);

    if (selectedEntity.has_value())
    {
        EntityID id = selectedEntity.value();
        if (CameraService::Get().enabled)
            CameraService::Get().LockOn(id);
        else
            CameraService::Get().Reset();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
}