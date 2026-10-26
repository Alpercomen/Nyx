#include "SceneLoader.h"

#include <fstream>

#include "ComponentRegistry.h"

#include <Application/Services/Managers/SceneManager/SceneManager.h>
#include <Application/Utils/MathUtils/MathUtils.h>

namespace Nyx
{
	HashMap<String, EntityID> SceneLoader::Load(Scene& scene, const String& jsonPath)
	{
		NameMap ids;

		std::ifstream file(jsonPath);
		if (!file)
		{
			spdlog::error("Could not open scene file '{}'", jsonPath);
			return ids;
		}

		const json doc = json::parse(file, nullptr, false);
		if (doc.is_discarded() || !doc.contains("entities") || !doc.at("entities").is_array())
		{
			spdlog::error("Scene file '{}' is not valid (expected {{ \"entities\": [ ... ] }}).", jsonPath);
			return ids;
		}

		Vector<std::pair<EntityID, const json*>> built;
		for (const json& e : doc.at("entities"))
		{
			if (!e.contains("name"))
			{
				spdlog::error("Scene file '{}': an entity has no \"name\". Skipped.", jsonPath);
				continue;
			}

			const String name = e.at("name").get<String>();
			if (ids.contains(name))
			{
				spdlog::error("Duplicate entity name '{}'. Skipped.", name);
				continue;
			}

			const EntityID id = scene.CreateEmptyEntity(name);
			ids[name] = id;
			built.emplace_back(id, &e);
		}

		for (const auto& [id, e] : built)
		{
			if (!e->contains("components"))
				continue;

			ComponentRegistry::Get().Apply(id, e->at("name").get<String>(), e->at("components"), ids);
		}

		spdlog::info("Loaded {} entities from '{}'", built.size(), jsonPath);
		return ids;
	}
}