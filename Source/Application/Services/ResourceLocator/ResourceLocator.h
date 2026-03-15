#pragma once

#include <spdlog/spdlog.h>
#include <Application/Core/Core.h>

namespace Nyx
{
	class ResourceLocator
	{
	public:
		static void Initialize(const String& executablePath)
		{
			FileSystem::path exePath = FileSystem::absolute(FileSystem::path(executablePath));
			m_projectRoot = exePath.parent_path();

			// Navigate upward to reach known project root if needed
			while (!FileSystem::exists(m_projectRoot / "Nyx") && m_projectRoot.has_parent_path())
			{
				m_projectRoot = m_projectRoot.parent_path();
			}

			spdlog::info("Project Root Found: {}", m_projectRoot.string());
		}

		static String Get(const String& relativePath)
		{
			FileSystem::path fullPath = m_projectRoot / relativePath;
			if (!FileSystem::exists(fullPath))
			{
				spdlog::error("Missing Resource: {}", fullPath.string());
			}

			return fullPath.string();
		}

	private:
		static inline FileSystem::path m_projectRoot;
	};
}