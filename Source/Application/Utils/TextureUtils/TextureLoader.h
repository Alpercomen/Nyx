#pragma once

#include <Application/Core/Core.h>

namespace Nyx
{
	struct TextureData
	{
		uint8_t* pixels = nullptr;
		int32 width = 0;
		int32 height = 0;
		int32 channels = 0;

		bool IsValid() const { return pixels != nullptr; }
	};

	class TextureLoader
	{
	public:
		static TextureData Load(const String& path, bool flipVertically = true);
		static void Free(TextureData& image);
	};
}