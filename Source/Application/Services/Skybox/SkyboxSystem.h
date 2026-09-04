#pragma once
#include <Application/Core/Core.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Renderer/Skybox/Skybox.h>

namespace Nyx
{
	class SkyboxSystem : public Singleton<SkyboxSystem>
	{
	public:
		void SetSkybox(const SharedPtr<Skybox> skybox)
		{
			m_skyboxPtr = skybox;
		}

		void Draw(const Camera& camera)
		{
			if (m_skyboxPtr)
				m_skyboxPtr->Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix());
		}

	private:
		SharedPtr<Skybox> m_skyboxPtr;
	};
}