#pragma once
#include <vector>

#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Camera/Camera.h>
#include <Application/Utils/ShaderUtils/ShaderUtils.h>

#include <Application/Core/Renderer/Renderer.h>

namespace Nyx 
{
    class Engine {
    public:
        Engine() = default;
        void EnsureGL();
        void EnsureFBO(int w, int h);
        void ResizeFBO(Math::Vec2f size, SharedPtr<Scene> s);
        void Render(Scene& scene);
        void SetGrid(bool enabled);

        GLuint GetSceneFBO() const { return m_sceneFBO; }

    private:
        bool m_glReady = false;
        bool m_fboReady = false;
        int  m_sceneTexWidth = 1280;
        int  m_sceneTexHeight = 720;

        GLuint m_sceneFBO = 0;
        GLuint m_sceneColorTex = 0;
        GLuint m_sceneDepthRBO = 0;

        Renderer m_Renderer;
    };

}

