#pragma once
#include <GLFW/glfw3.h>

#include <Application/Core/Core.h>

namespace Nyx
{
	struct RenderState
	{
		bool8 m_depthTest;
		bool8 m_blend;
		bool8 m_cullFace;

		int32 m_depthFunc;
		int32 m_blendSrc, m_blendDst;
		int32 m_cullMode;

		void Capture()
		{
			m_depthTest = glIsEnabled(GL_DEPTH_TEST);
			m_blend		= glIsEnabled(GL_BLEND);
			m_cullFace	= glIsEnabled(GL_CULL_FACE);

			glGetIntegerv(GL_DEPTH_FUNC, &m_depthFunc);
			glGetIntegerv(GL_BLEND_SRC_ALPHA, &m_blendSrc);
			glGetIntegerv(GL_BLEND_DST_ALPHA, &m_blendDst);
			glGetIntegerv(GL_CULL_FACE_MODE, &m_cullMode);
		}

		void Restore()
		{
			SetState(GL_DEPTH_TEST, m_depthTest);
			SetState(GL_BLEND, m_blend);
			SetState(GL_CULL_FACE, m_cullFace);

			glDepthFunc(m_depthFunc);
			glBlendFunc(m_blendSrc, m_blendDst);
			glCullFace(m_cullMode);

			glDepthMask(GL_TRUE);
		}

	private:
		static void SetState(GLenum capability, bool8 enabled)
		{
			enabled ? glEnable(capability) : glDisable(capability);
		}
	};

	class ImmediatePipeline : public Singleton<ImmediatePipeline>
	{
	public:
		void Begin()
		{
			m_state.Capture();
		}

		void End()
		{
			m_state.Restore();
		}

		void UseGrid()
		{
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_CLAMP);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthMask(GL_FALSE);
		}

		void UseSphere()
		{
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_CLAMP);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_TRUE);
		}

	private:
		RenderState m_state;
	};
}