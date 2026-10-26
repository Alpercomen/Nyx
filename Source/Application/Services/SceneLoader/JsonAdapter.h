#pragma once

#include <filesystem>

#include <Application/Core/Core.h>
#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Graphics/Lighting/LightingSystem.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Services/ModelImporter/ModelImporter.h>
#include <Application/Utils/MathUtils/MathUtils.h>

#include "ComponentRegistry.h"

namespace Nyx
{
	NLOHMANN_JSON_SERIALIZE_ENUM(LightType, { 
		{ LightType::POINT, "point" },
		{ LightType::DIRECTIONAL, "directional" },
		{ LightType::SPOT, "spot" }
	})

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(OrbitalElements, semiMajorAxis, eccentricity, inclination, longitudeAscendingNode, argumentOfPeriapsis, trueAnomaly)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LightComponent, type, color, intensity, range, decay)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AtmosphereComponent, color, radius, haloIntensity, haloAlpha, rimStart, rimEnd, lightSoftness)
}

namespace Nyx::BuiltIn
{
	inline void LoadTransform(EntityID id, const json& j, const NameMap&)
	{
		const Math::Vec3d position = j.value("position", Math::Vec3d(0.0));
		const Math::Vec3d rotation = j.value("rotationDeg", Math::Vec3d(0.0));

		const json& s = j.at("scale");
		const Scale scale = s.is_array()
			? Scale(s.at(0).get<float64>(), s.at(1).get<float64>(), s.at(2).get<float64>())
			: Scale(s.get<float64>());

		ECS::Get().AddComponent(id, Transform{
			Position(position),
			Rotation(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)),
			scale });
	}

	inline void LoadRigidbody(EntityID id, const json& j, const NameMap&)
	{
		Velocity angularVelocity;

		const Math::Vec3d local = j.value("angularVelocity", Math::Vec3d(0.0));
		if (local != Math::Vec3d(0.0))
		{
			if (const Transform* transform = ECS::Get().GetComponent<Transform>(id))
				angularVelocity = LocalToWorld(local, *transform);
			else
				spdlog::warn("Rigidbody.angularVelocity needs a Transform on the same entity. Ignored.");
		}

		ECS::Get().AddComponent(id, Rigidbody{ j.at("mass").get<float64>(), angularVelocity });
	}

	inline void LoadSphere(EntityID id, const json& j, const NameMap&)
	{
		const String path = j.at("texture").get<String>();

		SphereDesc desc;
		desc.texture = &ResourceManager::GetMipmappedTexture(std::filesystem::path(path).stem().string(), path);

		if (j.contains("emissiveColor"))    desc.emissiveColor = j.at("emissiveColor").get<Math::Vec3f>();
		if (j.contains("emissiveStrength")) desc.emissiveStrength = j.at("emissiveStrength").get<float32>();

		ECS::Get().AddComponent(id, Sphere{ desc });
	}

	inline void LoadModel(EntityID id, const json& j, const NameMap&)
	{
		Model model = ModelImporter::LoadFromFile(j.at("path").get<String>());
		ECS::Get().AddComponent(id, std::move(model));
	}

	inline void LoadTidallyLocked(EntityID id, const json& j, const NameMap& names)
	{
		const String target = j.at("target").get<String>();
		auto it = names.find(target);
		if (it == names.end())
		{
			spdlog::error("TidallyLocked: target '{}' not found.", target);
			return;
		}

		ECS::Get().AddComponent(id, TidallyLocked{ it->second });
	}

	inline void LoadOrbit(EntityID id, const json& j, const NameMap& names)
	{
		const String parent = j.at("parent").get<String>();
		auto it = names.find(parent);
		if (it == names.end())
		{
			spdlog::error("Orbit: parent '{}' not found.", parent);
			return;
		}

		if (j.contains("apoapsis"))
		{
			InitializeOrbitFromApsides(
				id, it->second,
				j.at("periapsis").get<float64>(),
				j.at("apoapsis").get<float64>(),
				j.value("inclination", 0.0),
				j.value("longitudeAscendingNode", 0.0),
				j.value("argumentOfPeriapsis", 0.0),
				j.value("startAtPeriapsis", false));
		}
		else
		{
			InitializeOrbit(id, it->second, j.get<OrbitalElements>());
		}
	}

	NYX_REGISTER_LOADER(Transform, "Transform", -100, LoadTransform);
	NYX_REGISTER_LOADER(Rigidbody, "Rigidbody", -50, LoadRigidbody);
	NYX_REGISTER_LOADER(Sphere, "Sphere", 0, LoadSphere);
	NYX_REGISTER_LOADER(Model, "Model", 0, LoadModel);
	NYX_REGISTER_LOADER(TidallyLocked, "TidallyLocked", 0, LoadTidallyLocked);
	NYX_REGISTER_LOADER(OrbitalElements, "Orbit", 100, LoadOrbit);

	NYX_REGISTER_COMPONENT(LightComponent, "Light");
	NYX_REGISTER_COMPONENT(AtmosphereComponent, "Atmosphere");
	NYX_REGISTER_TAG(OrbitTrail, "OrbitTrail");
}