#pragma once

#include <Application/Core/Core.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Graphics/Lighting/LightingSystem.h>
#include <Application/Resource/Components/Mesh/GridMesh/GridMesh.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Utils/MathUtils/MathUtils.h>
#include <Application/Constants/Constants.h>
#include <Application/Services/ModelImporter/ModelImporter.h>
#include <Application/Graphics/Skybox/Skybox.h>
#include <Application/Graphics/OrbitTrailSystem/OrbitTrailSystem.h>

#include <Application/Services/SceneLoader/SceneLoader.h>

namespace Nyx 
{
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

		void Draw(const Camera& camera, const Transform& cameraTransform)
		{
			const auto& transform = ECS::Get().GetComponent<Transform>(m_entityID);
			if (!transform)
				return;

			// Scale it down for rendering purposes
			auto pos = transform->position / METERS_PER_UNIT;
			auto sca = transform->scale / METERS_PER_UNIT;
			auto rot = transform->rotation;

			const Position& cameraPos = cameraTransform.position;
			Math::Vec3d relPos = Math::Vec3f(pos.GetWorld() - cameraPos.GetWorld());

			Math::Mat4f model = glm::translate(Math::Mat4d(1.0), relPos) * rot.ToMatrix() * sca.ToMatrix();
			Math::Mat4f view = camera.GetViewMatrix();
			Math::Mat4f projection = camera.GetProjectionMatrix();

			if (ECS::Get().HasComponent<Sphere>(m_entityID))
			{
				const auto& sphere = ECS::Get().GetComponent<Sphere>(m_entityID);
				sphere->DrawSphere(model, view, projection);
			}

			if (ECS::Get().HasComponent<Model>(m_entityID))
			{
				const auto& model3d = ECS::Get().GetComponent<Model>(m_entityID);
				model3d->DrawModel(model, view, projection);
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

		EntityID CreateModel(String name, const Transform& transform, const Rigidbody& rigidbody, const String& filePath)
		{
			SharedPtr<SceneObject> obj = MakeShared<SceneObject>(name);
			Model model = ModelImporter::LoadFromFile(filePath);

			ECS::Get().AddComponent(obj->GetEntityID(), std::move(model));
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

		EntityID CreateLight(String name, const Transform& transform, LightComponent& lightParameters)
		{
			SharedPtr<SceneObject> obj = MakeShared<SceneObject>(name);
			ECS::Get().AddComponent(obj->GetEntityID(), transform);
			ECS::Get().AddComponent(obj->GetEntityID(), lightParameters);

			m_sceneObjectPtrs[obj->GetEntityID()] = obj;
			return obj->GetEntityID();
		}

		void SetSkybox(SharedPtr<Skybox> skybox)
		{
			m_skyboxPtr = std::move(skybox);
		}

		const SharedPtr<Skybox>& GetSkybox() const
		{
			return m_skyboxPtr;
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
		SharedPtr<Skybox> m_skyboxPtr;
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
			m_scenes[sceneID] = std::move(newScene);
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

		void LoadSkybox(SceneID& sceneID)
		{
			Scene* scenePtr = GetScene(sceneID);

			if (scenePtr == nullptr)
				return;

			// Right, Left, Top, Bottom, Front, Back
			const Vector<String> faces = {
				R"(Nyx\Source\Assets\Textures\Skybox\26-09-05-00-05-02_Right.png)",
				R"(Nyx\Source\Assets\Textures\Skybox\26-09-05-00-05-02_Left.png)",
				R"(Nyx\Source\Assets\Textures\Skybox\26-09-05-00-05-02_Top.png)",
				R"(Nyx\Source\Assets\Textures\Skybox\26-09-05-00-05-02_Bottom.png)",
				R"(Nyx\Source\Assets\Textures\Skybox\26-09-05-00-05-02_Front.png)",
				R"(Nyx\Source\Assets\Textures\Skybox\26-09-05-00-05-02_Back.png)",
			};

			scenePtr->SetSkybox(MakeShared<Skybox>(faces));
		}

		void LoadEntities(SceneID& sceneID, const String& levelPath)
		{
			Scene* scenePtr = GetScene(sceneID);
			if (scenePtr == nullptr)
				return;

			scenePtr->CreateCamera("Camera", Transform{ Math::Vec3d(AU / METERS_PER_UNIT, 0.0f, 0.1f) });
			SceneLoader::Load(*scenePtr, levelPath);
		}

	private:
		SceneID m_activeSceneID = NO_ID;
		HashMap<SceneID, Scene> m_scenes;
	};
}