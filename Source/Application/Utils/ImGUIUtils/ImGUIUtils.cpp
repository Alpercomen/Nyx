#include <vector>
#include <iostream>

#include <spdlog/spdlog.h>
#include <Application/Utils/ImGUIUtils/ImGUIUtils.h>

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

    ImGui::Begin("Game View");
    ImVec2 textureSize = ImGui::GetContentRegionAvail(); // available ImGui space
    ImGui::Image(
        (ImTextureID)(intptr_t)sceneTextureID,
        textureSize,
        ImVec2(0, 1),  // uv0
        ImVec2(1, 0),  // uv1
        ImVec4(1, 1, 1, 1),  // tint (white)
        ImVec4(0, 0, 0, 0)   // border (none)
    );
    ImGui::End();

    return textureSize;
}

void ImGUIUtils::DrawSimulationInfo()
{
    auto& spheres = ECS::Get().GetAllComponents<Sphere>();
    auto& sphereIDs = ECS::Get().GetAllComponentIDs<Sphere>();

    auto& cameras = ECS::Get().GetAllComponents<Camera>();
    auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

    ImGui::Begin("Simulation Info");

    ImGui::Text("Objects: %d", static_cast<int>(spheres.size()));
    for (size_t i = 0; i < spheres.size(); ++i)
    {
        auto& sphere = spheres[i];
        const EntityID& id = sphereIDs[i];

        if (ECS::Get().HasComponent<Name>(id))
        {
            const auto& name = ECS::Get().GetComponent<Name>(id)->name;
            ImGui::Text("[%s]", name.data());
        }

        if (ECS::Get().HasComponent<Transform>(id))
        {
            auto& transform = *ECS::Get().GetComponent<Transform>(id);

            const auto& pos = transform.position.GetWorld();
            const auto& rot = transform.rotation.GetEulerAngles();
            const auto& sca = transform.scale.get();

            ImGui::Text("Pos: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
            ImGui::Text("Rot: (%.2f, %.2f, %.2f)", glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
            ImGui::Text("Sca: (%.2f, %.2f, %.2f)", sca.x, sca.y, sca.z);
        }

        if (ECS::Get().HasComponent<Rigidbody>(id))
        {

            const auto& rigidbody = *ECS::Get().GetComponent<Rigidbody>(id);

            const auto& velVec = rigidbody.velocity.GetWorld();
            const auto& accVec = rigidbody.acceleration.GetWorld();
            const auto& angularVel = rigidbody.angularVelocity.GetWorld();

            ImGui::Text("Vel: %.2f km/h", glm::length(velVec));
            ImGui::Text("Acc: %.2f km/h2", glm::length(accVec));
            ImGui::Text("Angular Vel: %.2f km/h", glm::length(angularVel));
        }
    }

    ImGui::Separator();
    ImGui::Text("Camera Info");

    for (size_t i = 0; i < cameras.size(); ++i)
    {
        auto& camera = cameras[i];
        const EntityID& id = cameraIDs[i];

        if (!ECS::Get().HasComponent<Transform>(id) || !ECS::Get().HasComponent<Name>(id))
            continue;

        auto& transform = *ECS::Get().GetComponent<Transform>(id);

        const auto& pos = transform.position;
        const auto& posVec = pos.GetWorld();

        const auto& name = ECS::Get().GetComponent<Name>(id)->name;

        ImGui::Text("[%s]", name.data());
        ImGui::Text("Pos: (%.2f, %.2f, %.2f)", posVec.x, posVec.y, posVec.z);
        ImGui::Text("Yaw: %.2f", camera.GetYaw());
        ImGui::Text("Pitch: %.2f", camera.GetPitch());
    }

    ImGui::End();
}

void ImGUIUtils::DrawSimulationControl(Engine* engine)
{
    ImGui::Begin("Simulation Control");
    ImGui::SliderFloat("Time Scale", &TIME_SCALE, 0.0f, 50000.0f, "%.8f", ImGuiSliderFlags_Logarithmic);
    ImGui::Checkbox("Show Grid", &engine->GetRenderer().m_gridEnabled);
    ImGui::End();
}

void ImGUIUtils::DrawWindow(Engine* enginePtr, Scene* scenePtr)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGUIUtils::InitDockableWindow();
    ImVec2 textureSize = ImGUIUtils::DrawGameWindow(enginePtr);
    ImGUIUtils::DrawSimulationInfo();
    ImGUIUtils::DrawSimulationControl(enginePtr);

    Math::Vec2f textureSizeVec = { (int)textureSize.x, (int)textureSize.y };
    enginePtr->ResizeFBO(textureSizeVec, scenePtr);

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