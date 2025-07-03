#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Application/Core/Core.h>
#include <Application/Utils/ShaderUtils/ShaderUtils.h>

namespace Nyx
{
	class Shader
	{
	public:
		Shader()
		{
			id = NO_ID;
		}

		Shader(const String& vertexPath, const String& fragmentPath)
		{
			id = CreateShaderProgram(vertexPath, fragmentPath);
		}

		void Use() const
		{
			glUseProgram(id);
		}

		uint32 GetID() const
		{
			return id;
		}

	private:
		uint32 id;
	};
}