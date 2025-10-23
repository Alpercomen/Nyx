#pragma once
#include <iostream>
#include <cmath>

#include <Application/Core/Engine/Engine.h>
#include <Application/Core/Services/Input/InputDispatcher.h>

#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Core/Physics/Physics.h>
#include <Application/Constants/Constants.h>

namespace Nyx 
{
    void Engine::EnsureGL() 
    {
        if (m_glReady) return;
        GLenum err = glewInit();
        if (err != GLEW_OK) throw std::runtime_error("glewInit failed");
        m_glReady = true;
    }

    static void MakeSceneFBO(GLuint& fbo, GLuint& color, GLuint& depth, int w, int h) 
    {
        glGenTextures(1, &color);
        glBindTexture(GL_TEXTURE_2D, color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenRenderbuffers(1, &depth);
        glBindRenderbuffer(GL_RENDERBUFFER, depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            spdlog::critical("Scene FBO incomplete");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Engine::EnsureFBO(int w, int h) 
    {
        EnsureGL();
        if (m_fboReady && w == m_sceneTexWidth && h == m_sceneTexHeight) 
            return;

        if (m_sceneFBO)      
            glDeleteFramebuffers(1, &m_sceneFBO);

        if (m_sceneColorTex) 
            glDeleteTextures(1, &m_sceneColorTex);

        if (m_sceneDepthRBO) 
            glDeleteRenderbuffers(1, &m_sceneDepthRBO);

        m_sceneTexWidth = w;  
        m_sceneTexHeight = h;

        MakeSceneFBO(m_sceneFBO, m_sceneColorTex, m_sceneDepthRBO, w, h);
        m_fboReady = true;
    }

    void Engine::ResizeFBO(Math::Vec2f newSize, SharedPtr<Scene> scenePtr) 
    {
        EnsureFBO((int)newSize.x, (int)newSize.y);
        auto& cam = *ECS::Get().GetComponent<Camera>(scenePtr->GetActiveCameraID());
        cam.SetAspectRatio((float)m_sceneTexWidth / (float)m_sceneTexHeight);
    }

    void Engine::Render(Scene& scene) 
    {
        EnsureFBO(m_sceneTexWidth, m_sceneTexHeight);

        glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
        glViewport(0, 0, m_sceneTexWidth, m_sceneTexHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Physics::Update(scene.GetActiveCameraID());
        m_Renderer.DrawScene(scene);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Engine::SetGrid(bool enabled)
    {
        enabled ? 
            spdlog::info("Grid enabled") : 
            spdlog::info("Grid disabled");

        m_Renderer.m_gridEnabled = enabled;
    }
}
