#pragma once

#include <Application/Core/Services/Managers/RenderManager/OpenGL.h>

namespace Nyx
{
	class Texture
	{
	public:
		Texture(const String& path, bool flipVertically = true);
		~Texture();

		void Bind(uint32 slot = 0);
		uint32 GetID() const { return m_textureID; }

	private:
		uint32 m_textureID = 0;
		int32 m_width = 0;
		int32 m_height = 0;
		int32 m_channels = 0;
	};
}