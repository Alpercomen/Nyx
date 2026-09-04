#pragma once

#include <gl/glew.h>

namespace Nyx
{
	class Texture
	{
	public:
		Texture(const String& path, bool flipVertically = true);
		Texture(const uint8* bytes, uint32 size, bool flipVertically = true);
		Texture(const uint8* pixels, uint32 width, uint32 height, uint32 channels);
		~Texture();

		void Bind(uint32 slot = 0) const;
		uint32 GetID() const { return m_textureID; }
		int32 GetWidth() const { return m_width; }
		int32 GetHeight() const { return m_height; }
		int32 GetChannels() const { return m_channels; }

	private:
		uint32 m_textureID = 0;
		int32 m_width = 0;
		int32 m_height = 0;
		int32 m_channels = 0;
	};
}