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
		Engine(void* windowPtr)
		{
			this->m_windowPtr = static_cast<GLFWwindow*>(windowPtr);

			InitFBO();
		}

		void InitFBO();
		void ResizeFBO(Math::Vec2f newSize, Scene* scenePtr);
		void Present(Scene& scene);

		GLuint GetSceneFBO() const { return m_sceneFBO; }
		GLuint GetSceneColorTex() const { return m_sceneColorTex; }
		GLuint GetSceneDepthRBO() const { return m_sceneDepthRBO; }
		Renderer& GetRenderer() { return m_Renderer; }

	private:
		Renderer m_Renderer;

		GLuint m_sceneFBO = 0;
		GLuint m_sceneColorTex = 0;
		GLuint m_sceneDepthRBO = 0;
		GLFWwindow* m_windowPtr;

		int m_sceneTexWidth = 1280;
		int m_sceneTexHeight = 720;
	};

}

