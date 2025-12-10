#pragma once

#include <Application/Core/Core.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Services/Lighting/LightingSystem.h>
#include <Application/Resource/Components/Mesh/GridMesh/GridMesh.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Utils/MathUtils/MathUtils.h>
#include <Application/Constants/Constants.h>

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
			Math::Vec3f relPos = Math::Vec3f(pos.GetWorld() - cameraPos.GetWorld());

			Math::Mat4f model = glm::translate(Math::Mat4f(1.0f), relPos) * rot.ToMatrix() * sca.ToMatrix();
			Math::Mat4f view = camera.GetViewMatrix();
			Math::Mat4f projection = camera.GetProjectionMatrix();

			Math::Mat4f mvp = projection * view * model;

			if (ECS::Get().HasComponent<Sphere>(m_entityID))
			{
				const auto& sphere = ECS::Get().GetComponent<Sphere>(m_entityID);
				sphere->DrawSphere(model, view, projection);
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

		EntityID CreateLight(String name, const Transform& transform, LightComponent& lightParameters)
		{
			SharedPtr<SceneObject> obj = MakeShared<SceneObject>(name);
			ECS::Get().AddComponent(obj->GetEntityID(), transform);
			ECS::Get().AddComponent(obj->GetEntityID(), lightParameters);

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
			earthDesc.texture = &ResourceManager::GetMipmappedTexture(
				"EarthTexture",
				R"(Nyx\Source\Assets\Textures\EarthTexture.jpg)"
			);

			SphereDesc moonDesc;
			moonDesc.texture = &ResourceManager::GetMipmappedTexture(
				"MoonTexture",
				R"(Nyx\Source\Assets\Textures\MoonTexture.jpg)"
			);

			SphereDesc mercuryDesc;
			mercuryDesc.texture = &ResourceManager::GetMipmappedTexture(
				"MercuryTexture",
				R"(Nyx\Source\Assets\Textures\MercuryTexture.jpg)"
			);

			SphereDesc sunDesc;
			sunDesc.emissiveColor = Math::Vec3f(1.0, 0.9, 0.7);
			sunDesc.emissiveStrength = 1.0;
			sunDesc.texture = &ResourceManager::GetMipmappedTexture(
				"SunTexture",
				R"(Nyx\Source\Assets\Textures\SunTexture.jpg)"
			);

			Position sunPosition(Math::Vec3f(0.0, 0.0, 0.0));
			Position earthPosition(Math::Vec3f(AU, 0.0, 0.0));
			Position moonPosition(Math::Vec3f(AU + EARTH_MOON_DISTANCE, 0.0, 0.0));
			Position mercuryPosition(Math::Vec3f(MERCURY_SUN_DISTANCE, 0.0, 0.0));

			Rotation sunRotation(0.0, 0.0, 0.0);
			Rotation earthRotation(0.0, 0.0, glm::radians(EARTH_INCLINATION));
			Rotation moonRotation(0.0, 0.0, 0.0);
			Rotation mercuryRotation(0.0, 0.0, 0.0);

			Scale earthSize(EARTH_RADIUS_EQUATORAL);
			Scale moonSize(MOON_RADIUS_EQUATORAL);
			Scale sunSize(SUN_RADIUS);
			Scale mercurySize(MERCURY_EQUATORAL_RADIUS);

			Transform earthTransform = Transform{ earthPosition, earthRotation, earthSize };
			Transform moonTransform = Transform{ moonPosition, moonRotation, moonSize };
			Transform sunTransform = Transform{ sunPosition, sunRotation, sunSize };
			Transform mercuryTransform = Transform{ mercuryPosition, mercuryRotation, mercurySize };

			Velocity earthAngularVelocity = LocalToWorld(Math::Vec3f(0.0, EARTH_ANGULAR_VELOCITY_RADIANS, 0.0), earthTransform);
			Velocity mercuryAngularVelocity = LocalToWorld(Math::Vec3f(0.0, MERCURY_ANGULAR_VELOCITY_RADIANS, 0.0), mercuryTransform);

			EntityID cameraID = scenePtr->CreateCamera("Camera", Transform{ Math::Vec3f(AU / METERS_PER_UNIT, 0.0f, 10.0f) });
			EntityID moonID = scenePtr->CreatePlanet("Moon", moonTransform, Rigidbody{ MOON_MASS }, moonDesc);
			EntityID earthID = scenePtr->CreatePlanet("Earth", earthTransform, Rigidbody{ EARTH_MASS , earthAngularVelocity }, earthDesc);
			EntityID sunID = scenePtr->CreatePlanet("Sun", sunTransform, Rigidbody{ SUN_MASS }, sunDesc);
			EntityID mercuryID = scenePtr->CreatePlanet("Mercury", mercuryTransform, Rigidbody{ MERCURY_MASS , mercuryAngularVelocity }, mercuryDesc);

			LightComponent pointLight;
			pointLight.type = LightType::POINT;
			pointLight.color = Math::Vec3f(1.0, 1.0, 1.0);
			pointLight.intensity = 1.0;
			pointLight.range = SOL_SYSTEM_RADIUS;
			pointLight.decay = 1 / SOL_SYSTEM_RADIUS;
			ECS::Get().AddComponent(sunID, pointLight);

			InitializeCircularOrbit(mercuryID, sunID, 0.0);
			InitializeCircularOrbit(earthID, sunID, 0.0);
			InitializeCircularOrbit(moonID, earthID, 0.0, true);
		}

	private:
		SceneID m_activeSceneID = NO_ID;
		HashMap<SceneID, Scene> m_scenes;
	};
}