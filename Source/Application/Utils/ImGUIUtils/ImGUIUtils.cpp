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
        auto& transform = *ECS::Get().GetComponent<Transform>(id);

        const auto& pos = transform.position.GetWorld();
        const auto& rot = transform.rotation.GetEulerAngles();
        const auto& sca = transform.scale.get();

        bool hasCamera = ECS::Get().HasComponent<Camera>(id);

        ImGui::Text("Pos: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        if (hasCamera)
        {
            const auto& camera = *ECS::Get().GetComponent<Camera>(id);
            ImGui::Text("Yaw: %.2f", camera.GetYaw());
            ImGui::Text("Pitch: %.2f", camera.GetPitch());
        }
        else
        {
            ImGui::Text("Rot: (%.2f, %.2f, %.2f)", glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
            ImGui::Text("Sca: (%.2f, %.2f, %.2f)", sca.x, sca.y, sca.z);
        }
    }
}

void ImGUIUtils::DrawRigidbody(EntityID& id)
{
    if (ECS::Get().HasComponent<Rigidbody>(id))
    {
        ImGui::Separator();
        const auto& rigidbody = *ECS::Get().GetComponent<Rigidbody>(id);

        const auto& velVec = rigidbody.velocity.GetWorld();
        const auto& accVec = rigidbody.acceleration.GetWorld();
        const auto& angularVel = rigidbody.angularVelocity.GetWorld();

        ImGui::Text("Vel: %.2f km/s", glm::length(velVec));
        ImGui::Text("Acc: %.2f km/s2", glm::length(accVec));
        ImGui::Text("Angular Vel: %.10f km/s", glm::length(angularVel));
    }
}

void ImGUIUtils::DrawAtmosphereComponent(EntityID& id)
{
    if (ECS::Get().HasComponent<AtmosphereComponent>(id))
    {
        ImGui::Separator();
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
            CameraService::Get().focusEnabled = false;
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