#pragma once
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Constants/Constants.h>


double GravitationalForce(double mu, double r) 
{
	return mu / (r * r);
}

double CalculateOrbitalVelocity(double otherMass, double r)
{
	return std::sqrt(G * otherMass / r);
}

double RotationDegreeToLinearVelocity(float degreesPerSecond, float radiusMeters)
{
    float radiansPerSecond = glm::radians(degreesPerSecond);
    return (radiansPerSecond * radiusMeters) / METERS_PER_UNIT;
}

void InitializeCircularOrbit(EntityID satelliteID, EntityID attractorID, bool isTidallyLocked) {
    // Ensure required components
    if (!ECS::Get().HasComponent<Transform>(satelliteID) ||
        !ECS::Get().HasComponent<Transform>(attractorID) ||
        !ECS::Get().HasComponent<Rigidbody>(satelliteID) ||
        !ECS::Get().HasComponent<Rigidbody>(attractorID))
    {
        spdlog::error("Missing required components to initialize orbit.");
        return;
    }

    if (ECS::Get().HasComponent<Name>(satelliteID) &&
        ECS::Get().HasComponent<Name>(attractorID))
    {
        const Name& satelliteName = *ECS::Get().GetComponent<Name>(satelliteID);
        const Name& attractorName = *ECS::Get().GetComponent<Name>(attractorID);

        if (isTidallyLocked)
        {
            ECS::Get().AddComponent(satelliteID, TidallyLocked{ attractorID });
            spdlog::info("{} is set to be tidally locked around the orbit of {}", satelliteName.name, attractorName.name);
        }
        else
        {
            spdlog::info("{} is set to orbit around {}", satelliteName.name, attractorName.name);
        }
    }


    auto& satellitePos = ECS::Get().GetComponent<Transform>(satelliteID)->position;
    auto& attractorPos = ECS::Get().GetComponent<Transform>(attractorID)->position;

    auto& satelliteRig = *ECS::Get().GetComponent<Rigidbody>(satelliteID);
    auto& attractorRig = *ECS::Get().GetComponent<Rigidbody>(attractorID);

    glm::vec3 direction = glm::normalize(attractorPos.GetWorld() - satellitePos.GetWorld());
    float distanceMeters = glm::length(attractorPos.GetWorld() - satellitePos.GetWorld());

    // Correct orbital speed calculation (SI units)
    double orbitalSpeed = std::sqrt(G * attractorRig.mass / distanceMeters);

    // Compute tangential direction
    glm::vec3 up = glm::vec3(0, 1, 0);

    if (std::abs(glm::dot(direction, up)) > 0.99f)
        up = glm::vec3(1, 0, 0); // avoid near-parallel vectors

    glm::vec3 tangential = glm::normalize(glm::cross(direction, up));
    glm::vec3 satelliteVel = tangential * static_cast<float>(orbitalSpeed);

    // Apply to satellite
    satelliteRig.velocity.SetWorld(attractorRig.velocity.GetWorld() + satelliteVel);

    // Conservation of momentum: Apply opposite to attractor
    glm::vec3 momentum = satelliteVel * static_cast<float>(satelliteRig.mass);
    glm::vec3 attractorDeltaVel = -momentum / static_cast<float>(attractorRig.mass);
    attractorRig.velocity.SetWorld(attractorRig.velocity.GetWorld() + attractorDeltaVel);

    spdlog::info("Initialized orbit:");
    spdlog::info(" - Satellite vel = ({:.2f}, {:.2f}, {:.2f})", satelliteVel.x, satelliteVel.y, satelliteVel.z);
    spdlog::info(" - Attractor vel = ({:.6f}, {:.6f}, {:.6f})", attractorDeltaVel.x, attractorDeltaVel.y, attractorDeltaVel.z);
}

void Attract(const EntityID& objID)
{
	if (!ECS::Get().HasComponent<Rigidbody>(objID) || !ECS::Get().HasComponent<Transform>(objID))
		return;

    auto sphereIDs = ECS::Get().GetAllComponentIDs<Sphere>();

	for (size_t i = 0; i < sphereIDs.size(); ++i)
	{
        const EntityID& id = sphereIDs[i];
		if (objID == id)
			continue;

        if (!ECS::Get().HasComponent<Rigidbody>(id) || !ECS::Get().HasComponent<Transform>(id))
            continue;

        auto& objTransform  = *ECS::Get().GetComponent<Transform>(objID);
        auto& objRigidbody  = *ECS::Get().GetComponent<Rigidbody>(objID);

		auto& obj2Transform  = *ECS::Get().GetComponent<Transform>(id);
		auto& obj2Rigidbody  = *ECS::Get().GetComponent<Rigidbody>(id);

		double dx = objTransform.position.GetWorld().x - obj2Transform.position.GetWorld().x;
		double dy = objTransform.position.GetWorld().y - obj2Transform.position.GetWorld().y;
		double dz = objTransform.position.GetWorld().z - obj2Transform.position.GetWorld().z;

		Math::Vec3d diff = glm::vec3(dx, dy, dz);
		float distance = glm::length(diff);
		Math::Vec3d unitVector = glm::normalize(diff);

		float Gforce = (G * objRigidbody.mass * obj2Rigidbody.mass) / (distance * distance);
		float acc = Gforce / obj2Rigidbody.mass;

        Math::Vec3d accVec(acc * unitVector.x, acc * unitVector.y, acc * unitVector.z);
		Acceleration attraction(accVec);

        obj2Rigidbody.acceleration = attraction;
        obj2Rigidbody.velocity.Accelerate(attraction);

        // If object is tidally locked to another object
        if (ECS::Get().HasComponent<TidallyLocked>(objID))
        {
            const auto& lockedEntityId = ECS::Get().GetComponent<TidallyLocked>(objID)->lockedEntity;

            if (lockedEntityId == id)
                ApplyTidalLock(objTransform, obj2Transform, objRigidbody);
        }

	}
}

// Make Ta tidally locked towards Tb
void ApplyTidalLock(Transform& Ta, Transform& Tb, Rigidbody& Ra)
{
    const Math::Vec3f& Pa = Ta.position.GetWorld();
    const Math::Vec3f& Pb = Tb.position.GetWorld();

    Math::Vec3f dir = Pb - Pa;
    if (glm::length2(dir) < 1e-12f)
        return;

    dir = glm::normalize(dir);

    Math::Vec3f up = Math::Vec3f(0, 1, 0);
    if (glm::abs(glm::dot(up, dir)) > 0.99f)
        up = Math::Vec3f(1, 0, 0);

    const Math::Vec3f x = -dir;
    const Math::Vec3f z = -glm::normalize(glm::cross(up, x));
    const Math::Vec3f y = glm::cross(z, x);

    const Math::Mat3f basis(x, y, z);
    Math::Quatf qWorld = glm::normalize(glm::quat_cast(basis));

    Ta.rotation.SetQuaternion(qWorld);
    Ra.angularVelocity.SetWorld(glm::vec3(0.0f));
}