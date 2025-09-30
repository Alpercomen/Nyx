#pragma once

#include <Application/Core/Core.h>
#include <Application/Utils/TextureUtils/TextureLoader.h>
#include "Texture.h"

namespace Nyx
{
	Texture::Texture(const String& path, bool flipVertically)
	{
        TextureData img = TextureLoader::Load(path, flipVertically);
        if (!img.IsValid()) {
            throw std::runtime_error("Failed to load texture: " + path);
        }

        // Assign first, then decide the format
        m_width = img.width;
        m_height = img.height;
        m_channels = img.channels;

        GLenum format = GL_RGB;
        if (m_channels == 1)      format = GL_RED;
        else if (m_channels == 3) format = GL_RGB;
        else if (m_channels == 4) format = GL_RGBA;

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        // For 1- or 3-channel data, avoid row padding issues
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0,
            format, GL_UNSIGNED_BYTE, img.pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Use bind-to-edit API in GL 3.3 (NOT glTextureParameteri)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Optional: swizzle for single-channel textures (visualization)
        // if (m_channels == 1) {
        //     GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        //     glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        // }

        glBindTexture(GL_TEXTURE_2D, 0);
        TextureLoader::Free(img);
	}

	Texture::~Texture()
	{
		if (m_textureID != 0)
		{
			glDeleteTextures(1, &m_textureID);
		}
	}

	void Texture::Bind(uint32 slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}
}