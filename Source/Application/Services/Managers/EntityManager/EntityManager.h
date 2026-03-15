#pragma once
#include <spdlog/spdlog.h>

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Transform/Position.h>

namespace Nyx {
	using EntityID = uint32_t;

	class EntityManager {
	public:
		EntityID CreateEntity() 
		{
			EntityID id;
			if (!m_freeList.empty())
			{
				id = m_freeList.back();
				m_freeList.pop_back();

				if (id == NO_ID) 
				{
					spdlog::warn("Attempted to reuse NO_ID (0). Skipping.");
					return CreateEntity(); // recursive call; will go to nextID instead
				}
			}
			else
			{
				id = m_nextID++;
			}

			m_alive[id] = true;
			return id;
		}

		void DestroyEntity(EntityID id)
		{
			if (id == NO_ID)
				return;

			m_alive[id] = false;
			m_freeList.push_back(id);
		}

		bool IsAlive(EntityID id) const
		{
			auto it = m_alive.find(id);
			return it != m_alive.end() && it->second;
		}

	private:
		EntityID m_nextID = NO_ID;
		Vector<EntityID> m_freeList;
		HashMap<EntityID, bool> m_alive;
	};

	struct IComponentPool
	{
		virtual void Remove(EntityID id) = 0;
		virtual ~IComponentPool() = default;
	};

	template<typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		void Add(EntityID id, const T& component)
		{
			assert(!Has(id)); // already done
			size_t index = components.size();
			components.push_back(component);
			entityToIndex[id] = index;
			indexToEntity.push_back(id);
			assert(entityToIndex[id] == index); // sanity check
			assert(index < components.size());  // valid range
		}

		void Remove(EntityID id) override
		{
			if (!Has(id))
				return; // TODO: Convert to an assert(Has(id));

			size_t index = entityToIndex[id];
			size_t last = components.size() - 1;

			std::swap(components[index], components[last]);
			std::swap(indexToEntity[index], indexToEntity[last]);
			entityToIndex[indexToEntity[index]] = index;

			components.pop_back();
			indexToEntity.pop_back();
			entityToIndex.erase(id);
		}

		bool Has(EntityID id) const
		{
			return entityToIndex.find(id) != entityToIndex.end();
		}

		T* Get(EntityID id)
		{
			auto it = entityToIndex.find(id);
			if (it != entityToIndex.end())
				return &components[it->second];

			return nullptr;
		}

		Vector<T>& GetAll() { return components; }

		Vector<EntityID>& GetEntityIDs() { return indexToEntity; }

	private:
		Vector<T> components;
		Vector<EntityID> indexToEntity;
		HashMap<EntityID, size_t> entityToIndex;
	};

	class ECS : public Singleton<ECS>
	{
	public:
		EntityID CreateEntity()
		{
			return m_entityManager.CreateEntity();
		}

		void DestroyEntity(EntityID id)
		{
			m_entityManager.DestroyEntity(id);
			for (auto& [_, pool] : m_componentPools)
			{
				pool->Remove(id);
			}
		}

		template<typename T>
		void AddComponent(EntityID id, const T& component)
		{
			GetOrCreatePool<T>().Add(id, component);
		}

		template<typename T>
		void RemoveComponent(EntityID id)
		{
			GetPool<T>()->Remove(id);
		}

		template<typename T>
		bool HasComponent(EntityID id) {
			if (GetPool<T>() == nullptr)
				return false;

			return GetPool<T>()->Has(id);
		}

		template<typename T>
		T* GetComponent(EntityID id) {
			return GetPool<T>()->Get(id);
		}

		template<typename T>
		Vector<T>& GetAllComponents() {
			return GetPool<T>()->GetAll();
		}

		template<typename T>
		Vector<EntityID>& GetAllComponentIDs()
		{
			return GetPool<T>()->GetEntityIDs();
		}

		template<typename... T>
		Vector<EntityID> View()
		{
			Vector<EntityID> result;

			// For now we assume the first component type T0 gives us a good pool to iterate
			if constexpr (sizeof...(T) > 0) {
				auto* pool = GetPool<std::tuple_element_t<0, std::tuple<T...>>>();
				if (pool == nullptr)
					return result;

				for (EntityID id : pool->GetEntityIDs()) {
					if ((HasComponent<T>(id) && ...)) {
						result.push_back(id);
					}
				}
			}

			return result;
		}

	private:
		template<typename T>
		ComponentPool<T>* GetPool()
		{
			auto it = m_componentPools.find(typeid(T));
			if (it == m_componentPools.end())
				return nullptr;
			return static_cast<ComponentPool<T>*>(it->second.get());
		}

		template<typename T>
		ComponentPool<T>& GetOrCreatePool()
		{
			auto it = m_componentPools.find(typeid(T));
			if (it == m_componentPools.end())
			{
				auto pool = std::make_unique<ComponentPool<T>>();
				m_componentPools[typeid(T)] = std::unique_ptr<IComponentPool>(pool.release());
			}
			
			return *static_cast<ComponentPool<T>*>(m_componentPools[typeid(T)].get());
		}

		EntityManager m_entityManager;
		HashMap<TypeIndex, UniquePtr<IComponentPool>> m_componentPools;

		template<typename T>
		static void DeletePool(void* ptr)
		{
			delete static_cast<ComponentPool<T>*>(ptr);
		}
	};
}