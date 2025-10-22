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

		void Draw(Camera& camera)
		{
			const auto& transform = ECS::Get().GetComponent<Transform>(m_entityID);
			if (!transform)
				return;

			// Scale it down for rendering purposes
			auto pos = transform->position / METERS_PER_UNIT;
			auto sca = transform->scale / METERS_PER_UNIT;
			auto rot = transform->rotation;

			Position cameraPos = camera.GetPosition();
			auto relPos = Math::Vec3f(pos.GetWorld() - cameraPos.GetWorld());

			Math::Mat4f model = glm::translate(Math::Mat4f(1.0f), relPos) * rot.ToMatrix() * sca.ToMatrix();
			Math::Mat4f view = camera.GetViewMatrix();
			Math::Mat4f projection = camera.GetProjectionMatrix();

			const float& farPlane = camera.GetFarPlane();

			if (ECS::Get().HasComponent<Sphere>(m_entityID))
			{
				const auto& sphere = ECS::Get().GetComponent<Sphere>(m_entityID);
				sphere->DrawSphere(model, view, projection, farPlane);
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

		EntityID CreateCamera(String name, const Position& position)
		{
			SharedPtr<SceneObject> obj = MakeShared<SceneObject>(name);

			Camera newCamera;
			newCamera.GetPosition() = position;

			ECS::Get().AddComponent(obj->GetEntityID(), std::move(newCamera));

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

			m_scenes[sceneID] = MakeShared<Scene>();
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

		SharedPtr<Scene> GetScene(SceneID& id)
		{
			auto it = m_scenes.find(id);
			if (it != m_scenes.end())
				return m_scenes[id];

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

		SharedPtr<Scene> GetActiveScene()
		{
			if (m_scenes.find(m_activeSceneID) != m_scenes.end())
				return m_scenes[m_activeSceneID];

			return nullptr;
		}

		SceneID GetActiveSceneID() const
		{
			return m_activeSceneID;
		}

		void GenerateEntities(SceneID sceneID)
		{
			SharedPtr<Scene> scenePtr = GetScene(sceneID);

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

			SphereDesc venusDesc;
			venusDesc.texture = &ResourceManager::GetMipmappedTexture(
				"VenusTexture",
				R"(Nyx\Source\Assets\Textures\VenusAtmosphere.jpg)"
			);

			SphereDesc marsDesc;
			marsDesc.texture = &ResourceManager::GetMipmappedTexture(
				"MarsTexture",
				R"(Nyx\Source\Assets\Textures\MarsTexture.jpg)"
			);

			SphereDesc jupiterDesc;
			jupiterDesc.texture = &ResourceManager::GetMipmappedTexture(
				"JupiterTexture",
				R"(Nyx\Source\Assets\Textures\JupiterTexture.jpg)"
			);

			SphereDesc saturnDesc;
			saturnDesc.texture = &ResourceManager::GetMipmappedTexture(
				"SaturnTexture",
				R"(Nyx\Source\Assets\Textures\SaturnTexture.jpg)"
			);

			SphereDesc uranusDesc;
			uranusDesc.texture = &ResourceManager::GetMipmappedTexture(
				"UranusTexture",
				R"(Nyx\Source\Assets\Textures\UranusTexture.jpg)"
			);

			SphereDesc neptuneDesc;
			neptuneDesc.texture = &ResourceManager::GetMipmappedTexture(
				"NeptuneTexture",
				R"(Nyx\Source\Assets\Textures\NeptuneTexture.jpg)"
			);

			SphereDesc sunDesc;
			sunDesc.emissiveColor = Math::Vec3f(1.0, 0.95, 0.8);
			sunDesc.emissiveStrength = 1.0;
			sunDesc.texture = &ResourceManager::GetMipmappedTexture(
				"SunTexture",
				R"(Nyx\Source\Assets\Textures\SunTexture.jpg)"
			);

			Position sunPosition(Math::Vec3f(0.0, 0.0, 0.0));
			Position earthPosition(Math::Vec3f(AU, 0.0, 0.0));
			Position moonPosition(Math::Vec3f(AU + EARTH_MOON_DISTANCE, 0.0, 0.0));
			Position mercuryPosition(Math::Vec3f(MERCURY_SUN_DISTANCE, 0.0, 0.0));
			Position venusPosition(Math::Vec3f(VENUS_SUN_DISTANCE, 0.0, 0.0));
			Position marsPosition(Math::Vec3f(MARS_SUN_DISTANCE, 0.0, 0.0));
			Position jupiterPosition(Math::Vec3f(JUPITER_SUN_DISTANCE, 0.0, 0.0));
			Position saturnPosition(Math::Vec3f(SATURN_SUN_DISTANCE, 0.0, 0.0));
			Position uranusPosition(Math::Vec3f(URANUS_SUN_DISTANCE, 0.0, 0.0));
			Position neptunePosition(Math::Vec3f(NEPTUNE_SUN_DISTANCE, 0.0, 0.0));

			Rotation sunRotation(0.0, 0.0, 0.0);
			Rotation earthRotation(0.0, 0.0, glm::radians(EARTH_AXIAL_TILT));
			Rotation moonRotation(0.0, 0.0, 0.0);
			Rotation mercuryRotation(0.0, 0.0, glm::radians(MERCURY_AXIAL_TILT));
			Rotation venusRotation(0.0, 0.0, glm::radians(VENUS_AXIAL_TILT));
			Rotation marsRotation(0.0, 0.0, glm::radians(MARS_AXIAL_TILT));
			Rotation jupiterRotation(0.0, 0.0, glm::radians(JUPITER_AXIAL_TILT));
			Rotation saturnRotation(0.0, 0.0, glm::radians(SATURN_AXIAL_TILT));
			Rotation uranusRotation(0.0, 0.0, glm::radians(URANUS_AXIAL_TILT));
			Rotation neptuneRotation(0.0, 0.0, glm::radians(NEPTUNE_AXIAL_TILT));

			Scale earthSize(EARTH_EQUATORAL_RADIUS);
			Scale moonSize(MOON_EQUATORAL_RADIUS);
			Scale sunSize(SUN_RADIUS);
			Scale mercurySize(MERCURY_EQUATORAL_RADIUS);
			Scale venusSize(VENUS_EQUATORAL_RADIUS);
			Scale marsSize(MARS_EQUATORAL_RADIUS);
			Scale jupiterSize(JUPITER_EQUATORAL_RADIUS);
			Scale saturnSize(SATURN_EQUATORAL_RADIUS);
			Scale uranusSize(URANUS_EQUATORAL_RADIUS);
			Scale neptuneSize(NEPTUNE_EQUATORAL_RADIUS);

			Transform earthTransform = Transform{ earthPosition, earthRotation, earthSize };
			Transform moonTransform = Transform{ moonPosition, moonRotation, moonSize };
			Transform sunTransform = Transform{ sunPosition, sunRotation, sunSize };
			Transform mercuryTransform = Transform{ mercuryPosition, mercuryRotation, mercurySize };
			Transform venusTransform = Transform{ venusPosition, venusRotation, venusSize };
			Transform marsTransform = Transform{ marsPosition, marsRotation, marsSize };
			Transform jupiterTransform = Transform{ jupiterPosition, jupiterRotation, jupiterSize };
			Transform saturnTransform = Transform{ saturnPosition, saturnRotation, saturnSize };
			Transform uranusTransform = Transform{ uranusPosition, uranusRotation, uranusSize };
			Transform neptuneTransform = Transform{ neptunePosition, neptuneRotation, neptuneSize };

			Velocity earthAngularVelocity = LocalToWorld(Math::Vec3f(0.0, EARTH_ANGULAR_VELOCITY_RADIANS, 0.0), earthTransform);
			Velocity mercuryAngularVelocity = LocalToWorld(Math::Vec3f(0.0, MERCURY_ANGULAR_VELOCITY_RADIANS, 0.0), mercuryTransform);
			Velocity venusAngularVelocity = LocalToWorld(Math::Vec3f(0.0, VENUS_ANGULAR_VELOCITY_RADIANS, 0.0), venusTransform);
			Velocity marsAngularVelocity = LocalToWorld(Math::Vec3f(0.0, MARS_ANGULAR_VELOCITY_RADIANS, 0.0), marsTransform);
			Velocity jupiterAngularVelocity = LocalToWorld(Math::Vec3f(0.0, JUPITER_ANGULAR_VELOCITY_RADIANS, 0.0), jupiterTransform);
			Velocity saturnAngularVelocity = LocalToWorld(Math::Vec3f(0.0, SATURN_ANGULAR_VELOCITY_RADIANS, 0.0), saturnTransform);
			Velocity uranusAngularVelocity = LocalToWorld(Math::Vec3f(0.0, URANUS_ANGULAR_VELOCITY_RADIANS, 0.0), uranusTransform);
			Velocity neptuneAngularVelocity = LocalToWorld(Math::Vec3f(0.0, NEPTUNE_ANGULAR_VELOCITY_RADIANS, 0.0), neptuneTransform);

			EntityID cameraID = scenePtr->CreateCamera("Camera", Position{ Math::Vec3f(AU / METERS_PER_UNIT, 0.0f, 1.0) });
			EntityID sunID = scenePtr->CreatePlanet("Sun", sunTransform, Rigidbody{ SUN_MASS }, sunDesc);
			EntityID mercuryID = scenePtr->CreatePlanet("Mercury", mercuryTransform, Rigidbody{ MERCURY_MASS , mercuryAngularVelocity }, mercuryDesc);
			EntityID venusID = scenePtr->CreatePlanet("Venus", venusTransform, Rigidbody{ VENUS_MASS, venusAngularVelocity }, venusDesc);
			EntityID earthID = scenePtr->CreatePlanet("Earth", earthTransform, Rigidbody{ EARTH_MASS , earthAngularVelocity }, earthDesc);
			EntityID moonID = scenePtr->CreatePlanet("Moon", moonTransform, Rigidbody{ MOON_MASS }, moonDesc);
			EntityID marsID = scenePtr->CreatePlanet("Mars", marsTransform, Rigidbody{ MARS_MASS, marsAngularVelocity }, marsDesc);
			EntityID jupiterID = scenePtr->CreatePlanet("Jupiter", jupiterTransform, Rigidbody{ JUPITER_MASS, jupiterAngularVelocity }, jupiterDesc);
			EntityID saturnID = scenePtr->CreatePlanet("Saturn", saturnTransform, Rigidbody{ SATURN_MASS, saturnAngularVelocity }, saturnDesc);
			EntityID uranusID = scenePtr->CreatePlanet("Uranus", uranusTransform, Rigidbody{ URANUS_MASS, uranusAngularVelocity }, uranusDesc);
			EntityID neptuneID = scenePtr->CreatePlanet("Neptune", neptuneTransform, Rigidbody{ NEPTUNE_MASS, neptuneAngularVelocity }, neptuneDesc);

			LightComponent pointLight;
			pointLight.type = LightType::POINT;
			pointLight.color = Math::Vec3f(1.0, 1.0, 1.0);
			pointLight.intensity = 1.0;
			pointLight.range = SOL_SYSTEM_RADIUS;
			pointLight.decay = 1 / SOL_SYSTEM_RADIUS;
			ECS::Get().AddComponent(sunID, pointLight);

			InitializeCircularOrbit(mercuryID, sunID, 0.0);
			InitializeCircularOrbit(venusID, sunID, 0.0);
			InitializeCircularOrbit(earthID, sunID, 0.0);
			InitializeCircularOrbit(moonID, earthID, 0.0, true);
			InitializeCircularOrbit(marsID, sunID, 0.0);
			InitializeCircularOrbit(jupiterID, sunID, 0.0);
			InitializeCircularOrbit(saturnID, sunID, 0.0);
			InitializeCircularOrbit(uranusID, sunID, 0.0);
			InitializeCircularOrbit(neptuneID, sunID, 0.0);
		}

	private:
		SceneID m_activeSceneID = NO_ID;
		HashMap<SceneID, SharedPtr<Scene>> m_scenes;
	};
}