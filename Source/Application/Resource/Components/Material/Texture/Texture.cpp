#include <Application/Core/Core.h>
#include <Application/Utils/TextureUtils/TextureLoader.h>
#include "Texture.h"

#include <stb_image.h>

namespace Nyx
{
    static void UploadTextureToGPU(uint32& textureID, int32 width, int32 height, int32 channels, const uint8* pixels)
    {
        GLenum format = GL_RGB;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    Texture::Texture(const String& path, bool flipVertically)
    {
        TextureData img = TextureLoader::Load(path, flipVertically);

        if (!img.IsValid())
        {
            throw std::runtime_error("Failed to load texture: " + path);
        }

        m_width = img.width;
        m_height = img.height;
        m_channels = img.channels;

        UploadTextureToGPU(m_textureID, m_width, m_height, m_channels, img.pixels);
        TextureLoader::Free(img);
    }

    Texture::Texture(const uint8* bytes, uint32 size, bool flipVertically)
    {
        stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);

        int width = 0;
        int height = 0;
        int channels = 0;
        stbi_uc* pixels = stbi_load_from_memory(bytes, static_cast<int>(size), &width, &height, &channels, 0);

        if (!pixels)
        {
            throw std::runtime_error("Failed to load embedded texture from memory.");
        }

        m_width = width;
        m_height = height;
        m_channels = channels;

        UploadTextureToGPU(m_textureID, m_width, m_height, m_channels, pixels);
        stbi_image_free(pixels);
    }

    Texture::Texture(const uint8* pixels, uint32 width, uint32 height, uint32 channels)
    {
        m_width = static_cast<int32>(width);
        m_height = static_cast<int32>(height);
        m_channels = static_cast<int32>(channels);

        GLenum format = GL_RGB;
        if (m_channels == 1)
            format = GL_RED;
        else if (m_channels == 3)
            format = GL_RGB;
        else if (m_channels == 4)
            format = GL_RGBA;

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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