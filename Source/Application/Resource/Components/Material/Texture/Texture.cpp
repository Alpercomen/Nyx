#include <Application/Core/Core.h>
#include "Texture.h"

#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <algorithm>

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

        GLint maxTexSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

        stbi_uc* uploadPixels = img.pixels;

        if ((m_width > maxTexSize) || (m_height > maxTexSize))
        {
            DownscaleAndUploadTexture(path, img, maxTexSize);
            spdlog::info("Loaded Texture from {} -> ID={} | w={} h={} ch={} (resized)", path, m_textureID, m_width, m_height, m_channels);
        }
        else
        {
            UploadTextureToGPU(m_textureID, m_width, m_height, m_channels, img.pixels);
            spdlog::info("Loaded Texture from {} -> ID={} | w={} h={} ch={}", path, m_textureID, m_width, m_height, m_channels);
            TextureLoader::Free(img);
        }
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
        spdlog::info("Loaded Texture from memory -> ID={} | w={} h={} ch={}", m_textureID, m_width, m_height, m_channels);
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

        spdlog::info("Loaded Texture from raw pixels -> ID={} | w={} h={} ch={}", m_textureID, m_width, m_height, m_channels);
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

    void Texture::DownscaleAndUploadTexture(const String& path, TextureData& img, GLint maxTexSize)
    {
        float32 scale = std::min((float32)maxTexSize / (float32)m_width, (float32)maxTexSize / (float32)m_height);
        int32 newW = std::max(1, static_cast<int32>(m_width * scale));
        int32 newH = std::max(1, static_cast<int32>(m_height * scale));

        spdlog::warn("Texture '{}' is larger than GL_MAX_TEXTURE_SIZE ({}). Resizing {}x{} -> {}x{}.", path, maxTexSize, m_width, m_height, newW, newH);

        size_t newSize = static_cast<size_t>(newW) * static_cast<size_t>(newH) * static_cast<size_t>(m_channels);
        stbi_uc* resized = new stbi_uc[newSize];

        for (int32 y = 0; y < newH; ++y)
        {
            int32 srcY = std::min(m_height - 1, static_cast<int32>(y / (float32)newH * m_height));
            for (int32 x = 0; x < newW; ++x)
            {
                int32 srcX = std::min(m_width - 1, static_cast<int32>(x / (float32)newW * m_width));
                for (int32 c = 0; c < m_channels; ++c)
                {
                    resized[(y * newW + x) * m_channels + c] = img.pixels[(srcY * m_width + srcX) * m_channels + c];
                }
            }
        }

        UploadTextureToGPU(m_textureID, newW, newH, m_channels, resized);

        TextureLoader::Free(img);
        delete[] resized;

        m_width = newW;
        m_height = newH;
    }
}