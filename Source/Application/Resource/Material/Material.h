#pragma once

#include <Application/Resource/Material/ShaderProgram/ShaderProgram.h>
#include <Application/Resource/Material/Texture/Texture.h>

namespace Nyx
{
	class Material
	{
	public:
		Material() : Material(Shader(), Math::Vec3f(0.8, 0.8, 0.8)) {};
		Material (Shader shader, Math::Vec3f baseColor, Texture* texture = nullptr) : m_shader(shader), m_baseColor(baseColor), m_texture(texture) {}

		void Bind()
		{
			m_shader.Use();

			bool hasTexture = m_texture && m_texture->GetID() != 0;

			glActiveTexture(GL_TEXTURE0);

			if (hasTexture)
			{
				glBindTexture(GL_TEXTURE_2D, m_texture->GetID());
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			}
			else
			{
				// Bind fallback white texture
				static GLuint fallbackTexture = 0;

				uint8_t whitePixel[3] = { 255, 255, 255 };

				glGenTextures(1, &fallbackTexture);
				glBindTexture(GL_TEXTURE_2D, fallbackTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			// Set texture uniform
			GLuint texLoc = glGetUniformLocation(m_shader.GetID(), "uTexture");
			glUniform1i(texLoc, 0);

			// Send hasTexture flag
			GLuint hasTexLoc = glGetUniformLocation(m_shader.GetID(), "uHasTexture");
			glUniform1i(hasTexLoc, hasTexture);

			// Send base color tint
			GLuint colorLoc = glGetUniformLocation(m_shader.GetID(), "uBaseColor");
			glUniform3fv(colorLoc, 1, glm::value_ptr(m_baseColor));
		}

		Shader& GetShader() { return m_shader; }

		void SetTexture(Texture* texture) { m_texture = texture; }
		void SetBaseColor(const Math::Vec3f& color) { m_baseColor = color; }

	private:
		Shader m_shader;
		Math::Vec3f m_baseColor;
		Texture* m_texture;
	};
}