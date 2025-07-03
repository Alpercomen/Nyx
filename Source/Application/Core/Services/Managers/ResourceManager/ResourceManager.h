#pragma once

#include <Application/Core/Core.h>
#include <Application/Constants/Constants.h>
#include <Application/Resource/Material/ShaderProgram/ShaderProgram.h>
#include <Application/Resource/Material/Texture/Texture.h>

namespace Nyx
{
	class ResourceManager
	{
	public:
		static Shader& GetShader(const String& name, const String& vertexPath = "", const String& fragmentPath = "");
		static Texture& GetTexture(const String& name, const String& texturePath = "");
		static void Clear();

	private:
		static inline HashMap<String, UniquePtr<Shader>> m_shaders;
		static inline HashMap<String, UniquePtr<Texture>> m_textures;
	};

}