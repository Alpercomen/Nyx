#pragma once
#include <gl/glew.h>

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Material/Shader/Shader.h>
#include <Application/Services/Managers/ResourceManager/ResourceManager.h>
#include <Application/Utils/TextureUtils/TextureLoader.h>
#include <Application/Services/ResourceLocator/ResourceLocator.h>
#include <Application/Core/Definitions/GlmNames.h>
#include <Application/Services/Pipeline/Immediate/Immediate.h>

namespace Nyx
{
	class Skybox
	{
	public:
		Skybox(const Vector<String>& faces = {});
		Skybox(const Skybox&) = delete;
		Skybox(Skybox&& other) noexcept;
		~Skybox();

		Skybox& operator=(Skybox&& other) noexcept;
		Skybox& operator=(const Skybox&) = delete;

		void Draw(const Math::Mat4f& view, const Math::Mat4f& proj) const;

	private:
		GLuint m_vao = 0;
		GLuint m_cubemap = 0;
		Shader m_shader;
	};
}