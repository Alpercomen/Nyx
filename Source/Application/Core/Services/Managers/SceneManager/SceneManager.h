#pragma once

#include <Application/Core/Core.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Resource/Components/Mesh/GridMesh/GridMesh.h>
#include <Application/Constants/Constants.h>

namespace Nyx {
	using SceneID = uint32;
	using CameraID = uint32;

	class SceneObject
	{
	public:
		SceneObject(String name)
		{
			m_entityID = ECS::Get().CreateEntity();
			ECS::Get().AddComponent(m_entityID, Name{ name });
		}

		~SceneObject()
		{
			ECS::Get().DestroyEntity(m_entityID);
		}

		EntityID GetEntityID()
		{
			return m_entityID;
		}

		void Draw(const Camera& camera)
		{
			const auto& transform = ECS::Get().GetComponent<Transform>(m_entityID);
			if (!transform)
				return;

			const auto& pos = transform->position;
			const auto& rot = transform->rotation;
			const auto& sca = transform->scale;

			Math::Mat4f model = glm::translate(Math::Mat4f(1.0f), pos.GetWorld()) * rot.toMatrix() * sca.toMatrix();
			Math::Mat4f view = camera.GetViewMatrix();
			Math::Mat4f projection = camera.GetProjectionMatrix();

			Math::Mat4f mvp = projection * view * model;

			if (ECS::Get().HasComponent<Sphere>(m_entityID))
			{
				const auto& sphere = ECS::Get().GetComponent<Sphere>(m_entityID);
				sphere->DrawSphere(mvp);
			}
		}

	private:
		EntityID m_entityID;
	};

	class Scene 
	{
	public:
		Scene() = default;
		~Scene() = default;

		CameraID GetActiveCameraID() const
		{
			if (m_activeCameraID != NO_ID && ECS::Get().HasComponent<Camera>(m_activeCameraID))
				return m_activeCameraID;

			return NO_ID;
		}

		EntityID CreateEmptyEntity(String name)
		{
			SharedPtr<SceneObject> obj = MakeShared<SceneObject>(name);

			m_sceneObjectPtrs[obj->GetEntityID()] = obj;
			return obj->GetEntityID();
		}

		EntityID CreatePlanet(String name, const Transform& transform, const Rigidbody& rigidbody, const SphereDesc& sphereDesc)
		{
			SharedPtr<SceneObject> obj = MakeShared<SceneObject>(name);

			ECS::Get().AddComponent(obj->GetEntityID(), Sphere{sphereDesc});
			ECS::Get().AddComponent(obj->GetEntityID(), transform);
			ECS::Get().AddComponent(obj->GetEntityID(), rigidbody);

			m_sceneObjectPtrs[obj->GetEntityID()] = obj;
			return obj->GetEntityID();
		}

		EntityID CreateCamera(String name, const Transform& transform)
		{
			SharedPtr<SceneObject> obj = MakeShared<SceneObject>(name);
			ECS::Get().AddComponent(obj->GetEntityID(), Camera{});
			ECS::Get().AddComponent(obj->GetEntityID(), transform);

			// Only set if valid
			if (ECS::Get().HasComponent<Camera>(obj->GetEntityID()))
			{
				m_activeCameraID = obj->GetEntityID();
			}
			else 
			{
				m_activeCameraID = NO_ID;
			}

			m_sceneObjectPtrs[obj->GetEntityID()] = obj;
			return obj->GetEntityID();
		}

		SharedPtr<SceneObject> GetSceneObject(const EntityID& entityID)
		{
			if (m_sceneObjectPtrs.find(entityID) != m_sceneObjectPtrs.end())
				return m_sceneObjectPtrs[entityID];

			spdlog::critical("SceneObject with given ID does not exist!");
			return nullptr;
		}

		uint32 GetSceneObjectSize() { return m_sceneObjectPtrs.size(); }

	private:
		CameraID m_activeCameraID = NO_ID;
		HashMap<EntityID, SharedPtr<SceneObject>> m_sceneObjectPtrs;
	};

	class SceneManager 
	{
	public:
		SceneID CreateScene()
		{
			SceneID sceneID = 1;
			while (m_scenes.contains(sceneID)) 
				++sceneID;

			if (sceneID == NO_ID) 
			{
				spdlog::critical("SceneID overflow: exhausted all SceneIDs!");
				return NO_ID;
			}

			Scene newScene;
			m_scenes[sceneID] = newScene;
			m_activeSceneID = sceneID;

			return sceneID;
		}

		void DeleteScene(SceneID& id)
		{
			auto it = m_scenes.find(id);

			if (it == m_scenes.end()) 
			{
				spdlog::critical("Could not delete the scene with the given ID = {} ", id);
				return;
			}
			
			m_scenes.erase(it);
		}

		Scene* GetScene(SceneID& id)
		{
			auto it = m_scenes.find(id);
			if (it != m_scenes.end())
				return &m_scenes[id];

			spdlog::critical("Could not get the scene with the given ID = {} ", id);
			return nullptr;
		}

		void SetActiveScene(SceneID id) 
		{
			if (m_scenes.find(id) != m_scenes.end())
				m_activeSceneID = id;
			else
				spdlog::error("Scene ID {} does not exist!", id);
		}

		Scene* GetActiveScene() 
		{
			if (m_scenes.find(m_activeSceneID) != m_scenes.end())
				return &m_scenes[m_activeSceneID];
			return nullptr;
		}

		void GenerateEntities(SceneID& sceneID)
		{
			Scene* scenePtr = GetScene(sceneID);

			if (scenePtr == nullptr)
				return;

			SphereDesc earthDesc;
			earthDesc.res = 250;
			earthDesc.texture = &ResourceManager::GetMipmappedTexture(
				"EarthTexture",
				R"(Nyx\Source\Assets\Textures\EarthTexture.jpg)"
			);

			SphereDesc moonDesc;
			moonDesc.res = 250;
			moonDesc.texture = &ResourceManager::GetMipmappedTexture(
				"MoonTexture",
				R"(Nyx\Source\Assets\Textures\MoonTexture.jpg)"
			);

			Transform earthTransform = Transform{ Position{}, Rotation{}, Scale{ 250.0 } };
			Transform moonTransform = Transform{ Math::Vec3f(3844.00, 0.0f, 0.0f), Rotation{}, Scale{ 60.0 } };

			EntityID earthID = scenePtr->CreatePlanet("Earth", earthTransform, Rigidbody{ 5.972e24 }, earthDesc);
			EntityID moonID = scenePtr->CreatePlanet("Moon", moonTransform, Rigidbody{ 7.342e22 }, moonDesc);
			EntityID cameraID = scenePtr->CreateCamera("Camera", Transform{ glm::vec3(0.0f, 0.0f, 1000.0f) });

			InitializeCircularOrbit(moonID, earthID);
		}

	private:
		SceneID m_activeSceneID = NO_ID;
		HashMap<SceneID, Scene> m_scenes;
	};
}