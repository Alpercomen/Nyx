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

    static void MakeSceneFBO(GLuint& fbo, GLuint& color, GLuint& depth, int w, int h) 
    {
        GL::Get()->glGenTextures(1, &color);
        GL::Get()->glBindTexture(GL_TEXTURE_2D, color);
        GL::Get()->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        GL::Get()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL::Get()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GL::Get()->glGenRenderbuffers(1, &depth);
        GL::Get()->glBindRenderbuffer(GL_RENDERBUFFER, depth);
        GL::Get()->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

        GL::Get()->glGenFramebuffers(1, &fbo);
        GL::Get()->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        GL::Get()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
        GL::Get()->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

        if (GL::Get()->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            spdlog::critical("Scene FBO incomplete");

        GL::Get()->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Engine::EnsureFBO(int w, int h) 
    {
        if (m_fboReady && w == m_sceneTexWidth && h == m_sceneTexHeight) 
            return;

        if (m_sceneFBO)      
            GL::Get()->glDeleteFramebuffers(1, &m_sceneFBO);

        if (m_sceneColorTex) 
            GL::Get()->glDeleteTextures(1, &m_sceneColorTex);

        if (m_sceneDepthRBO) 
            GL::Get()->glDeleteRenderbuffers(1, &m_sceneDepthRBO);

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

        GL::Get()->glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
        GL::Get()->glViewport(0, 0, m_sceneTexWidth, m_sceneTexHeight);
        GL::Get()->glClearColor(0, 0, 0, 1);
        GL::Get()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Physics::Update(scene.GetActiveCameraID());
        m_Renderer.DrawScene(scene);

        GL::Get()->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Engine::SetGrid(bool enabled)
    {
        enabled ? 
            spdlog::info("Grid enabled") : 
            spdlog::info("Grid disabled");

        m_Renderer.m_gridEnabled = enabled;
    }
}
