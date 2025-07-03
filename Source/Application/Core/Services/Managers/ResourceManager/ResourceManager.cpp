#pragma once

#include "ResourceManager.h"
#include <Application/Core/Services/ResourceLocator/ResourceLocator.h>

#include <spdlog/spdlog.h>

namespace Nyx
{
	Shader& ResourceManager::GetShader(const String& name, const String& vertexPath, const String& fragmentPath)
	{
		auto it = m_shaders.find(name);
		if (it != m_shaders.end())
		{
			return *(it->second);
		}

		if (vertexPath.empty() || fragmentPath.empty())
		{
			throw std::runtime_error("Shader '" + name + "' not loaded and no paths provided!");
		}

		String vPath = ResourceLocator::Get(vertexPath);
		String fPath = ResourceLocator::Get(fragmentPath);

		auto shader = MakeUnique<Shader>(vPath.data(), fPath.data());
		spdlog::info("Loaded Shader: {}", name);
		m_shaders[name] = std::move(shader);
		return *(m_shaders[name]);
	}

	Texture& ResourceManager::GetTexture(const String& name, const String& texturePath)
	{
		auto it = m_textures.find(name);
		if (it != m_textures.end())
		{
			return *(it->second);
		}

		if (texturePath.empty())
		{
			throw std::runtime_error("Texture '" + name + "' not loaded and no paths provided!");
		}

		String path = ResourceLocator::Get(texturePath);

		auto texture = MakeUnique<Texture>(path);
		spdlog::info("Loaded Texture: {}", name);
		m_textures[name] = std::move(texture);
		return *(m_textures[name]);
	}

	void ResourceManager::Clear()
	{
		spdlog::info("Clearing all cached resources");
		m_shaders.clear();
		m_textures.clear();
	}
}