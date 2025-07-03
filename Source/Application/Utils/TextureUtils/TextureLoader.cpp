#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "TextureLoader.h"

namespace Nyx
{
	TextureData TextureLoader::Load(const String& path, bool flipVertically)
	{
		stbi_set_flip_vertically_on_load(flipVertically);

		TextureData img{};

		img.pixels = stbi_load(path.data(), &img.width, &img.height, &img.channels, 0);
		if (!img.pixels)
		{
			throw std::runtime_error("Failed to load image:" + path);
		}

		return img;
	}

	void TextureLoader::Free(TextureData& img)
	{
		stbi_image_free(img.pixels);
		img.pixels = nullptr;
	}
}