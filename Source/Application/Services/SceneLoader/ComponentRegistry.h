#pragma once

#include <algorithm>
#include <functional>

#include <nlohmann/json.hpp>

#include <Application/Core/Core.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>

// ---------------------------------------------------------------- glm <-> json ([x, y, z])
namespace nlohmann
{
	template <typename T, glm::qualifier Q>
	struct adl_serializer<glm::vec<3, T, Q>>
	{
		template <typename BasicJsonType>
		static void from_json(const BasicJsonType& j, glm::vec<3, T, Q>& v)
		{
			v = glm::vec<3, T, Q>(
				j.at(0).template get<T>(),
				j.at(1).template get<T>(),
				j.at(2).template get<T>());
		}

		template <typename BasicJsonType>
		static void to_json(BasicJsonType& j, const glm::vec<3, T, Q>& v)
		{
			j = BasicJsonType::array({ v.x, v.y, v.z });
		}
	};
}

namespace Nyx
{
	using json = nlohmann::ordered_json;
	using NameMap = HashMap<String, EntityID>;

	class ComponentRegistry
	{
	public:
		using Loader = std::function<void(EntityID, const json&, const NameMap&)>;

		static ComponentRegistry& Get()
		{
			static ComponentRegistry instance;
			return instance;
		}

		bool Register(const String& key, int order, Loader loader)
		{
			const bool added = m_entries.emplace(key, Entry{ order, std::move(loader) }).second;
			if (!added)
				spdlog::error("Component key '{}' was registered twice!", key);
			return added;
		}

		template<typename T>
		bool RegisterComponent(const String& key, int order = 0)
		{
			return Register(key, order, [](EntityID id, const json& j, const NameMap&)
				{
					ECS::Get().AddComponent(id, j.get<T>());
				});
		}

		template<typename T>
		bool RegisterTag(const String& key, int order = 0)
		{
			return Register(key, order, [](EntityID id, const json&, const NameMap&)
				{
					ECS::Get().AddComponent(id, T{});
				});
		}

		void Apply(EntityID id, const String& entityName, const json& components, const NameMap& names) const
		{
			Vector<String> keys;
			for (auto& item : components.items())
			{
				if (!m_entries.contains(item.key()))
				{
					spdlog::warn("Entity '{}': unknown component '{}' (is its header included?)", entityName, item.key());
					continue;
				}
				keys.push_back(item.key());
			}

			std::stable_sort(keys.begin(), keys.end(), [this](const String& a, const String& b)
				{
					return m_entries.at(a).order < m_entries.at(b).order;
				});

			for (const String& key : keys)
			{
				try
				{
					m_entries.at(key).load(id, components.at(key), names);
				}
				catch (const json::exception& e)
				{
					spdlog::error("Entity '{}', component '{}': {}", entityName, key, e.what());
				}
			}
		}

	private:
		struct Entry
		{
			int    order;
			Loader load;
		};

		HashMap<String, Entry> m_entries;
	};
}

#define NYX_REGISTER_COMPONENT(Type, Key) \
	inline const bool kRegistered_##Type = ::Nyx::ComponentRegistry::Get().RegisterComponent<Type>(Key)

#define NYX_REGISTER_TAG(Type, Key) \
	inline const bool kRegistered_##Type = ::Nyx::ComponentRegistry::Get().RegisterTag<Type>(Key)

#define NYX_REGISTER_LOADER(Name, Key, Order, Function) \
	inline const bool kRegistered_##Name = ::Nyx::ComponentRegistry::Get().Register(Key, Order, Function)