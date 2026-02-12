#pragma once
#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <Application/Core/Engine/Engine.h>
#include <Application/Core/Services/Input/InputDispatcher.h>
#include <Application/Core/Services/Managers/SceneManager/SceneManager.h>

#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Utils/ImGUIUtils/ImGUIUtils.h>
#include <Application/Core/Physics/Physics.h>

#include <Application/Constants/Constants.h>

using namespace ImGUIUtils;

namespace Nyx {
    void Engine::InitFBO()
    {
        // Create texture
        glGenTextures(1, &m_sceneColorTex);
        glBindTexture(GL_TEXTURE_2D, m_sceneColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_sceneTexWidth, m_sceneTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Create renderbuffer for depth
        glGenRenderbuffers(1, &m_sceneDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_sceneDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, m_sceneTexWidth, m_sceneTexHeight);

        // Create framebuffer
        glGenFramebuffers(1, &m_sceneFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_sceneColorTex, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_sceneDepthRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            spdlog::critical("ERROR: Framebuffer not complete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Engine::ResizeFBO(Math::Vec2f newSize, Scene* scenePtr)
    {
        if (newSize.x == m_sceneTexWidth && newSize.y == m_sceneTexHeight)
            return;

        m_sceneTexWidth = newSize.x;
        m_sceneTexHeight = newSize.y;

        Camera& camera = *ECS::Get().GetComponent<Camera>(scenePtr->GetActiveCameraID());
        camera.SetAspectRatio((float32)m_sceneTexWidth / (float32)m_sceneTexHeight);

        glDeleteFramebuffers(1, &m_sceneFBO);
        glDeleteTextures(1, &m_sceneColorTex);
        glDeleteRenderbuffers(1, &m_sceneDepthRBO);
    
        InitFBO(); // Recreate with new size
    }

    void Engine::Present(Scene& scene)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);

        glViewport(0, 0, m_sceneTexWidth, m_sceneTexHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Physics::Update(DELTA_TIME);
        m_Renderer.DrawScene(scene);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGUIUtils::DrawWindow(this, &scene);
    }
}
