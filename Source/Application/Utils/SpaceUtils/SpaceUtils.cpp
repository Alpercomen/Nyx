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

void InitializeCircularOrbit(EntityID satelliteID, EntityID attractorID, float32 inclination, bool isTidallyLocked) {
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

    Math::Vec3f direction = glm::normalize(attractorPos.GetWorld() - satellitePos.GetWorld());
    float distanceMeters = glm::length(attractorPos.GetWorld() - satellitePos.GetWorld());

    // Correct orbital speed calculation (SI units)
    double orbitalSpeed = std::sqrt(G * attractorRig.mass / distanceMeters);

    // Compute tangential direction
    Math::Vec3f up = Math::Vec3f(0, 1, 0);

    if (std::abs(glm::dot(direction, up)) > 0.99f)
        up = Math::Vec3f(1, 0, 0); // avoid near-parallel vectors

    Math::Vec3f tangential = glm::normalize(glm::cross(direction, up));
    Math::Vec3f satelliteVel = tangential * static_cast<float>(orbitalSpeed);

    // Apply to satellite
    satelliteRig.velocity.SetWorld(attractorRig.velocity.GetWorld() + satelliteVel);

    // Conservation of momentum: Apply opposite to attractor
    Math::Vec3f momentum = satelliteVel * static_cast<float>(satelliteRig.mass);
    Math::Vec3f attractorDeltaVel = -momentum / static_cast<float>(attractorRig.mass);
    attractorRig.velocity.SetWorld(attractorRig.velocity.GetWorld() + attractorDeltaVel);

    spdlog::info("Initialized orbit:");
    spdlog::info(" - Satellite vel = ({:.2f}, {:.2f}, {:.2f})", satelliteVel.x, satelliteVel.y, satelliteVel.z);
    spdlog::info(" - Attractor vel = ({:.6f}, {:.6f}, {:.6f})", attractorDeltaVel.x, attractorDeltaVel.y, attractorDeltaVel.z);
}

void Attract(EntityID& cameraID)
{
    auto& ids = ECS::Get().GetAllComponentIDs<Rigidbody>();

    for (auto id : ids)
    {
        auto& rb = *ECS::Get().GetComponent<Rigidbody>(id);
        rb.acceleration = Math::Vec3f(0.0);
    }

    // --- compute pairwise accelerations ---
    for (size_t i = 0; i < ids.size(); ++i)
    {
        const EntityID& aID = ids[i];
        if (!ECS::Get().HasComponent<Rigidbody>(aID) || !ECS::Get().HasComponent<Transform>(aID))
            continue;

        auto& aBody = *ECS::Get().GetComponent<Rigidbody>(aID);
        auto& aTransform = *ECS::Get().GetComponent<Transform>(aID);

        const Math::Vec3d posA = aTransform.position.GetWorld();

        for (size_t j = i + 1; j < ids.size(); ++j)
        {
            const EntityID& bID = ids[j];
            if (!ECS::Get().HasComponent<Rigidbody>(bID) || !ECS::Get().HasComponent<Transform>(bID))
                continue;

            auto& bBody = *ECS::Get().GetComponent<Rigidbody>(bID);
            auto& bTransform = *ECS::Get().GetComponent<Transform>(bID);

            const Math::Vec3d posB = bTransform.position.GetWorld();

            Math::Vec3d delta = posB - posA;
            double distSq = glm::dot(delta, delta) + 1e-6;  // prevent div/0
            double dist = sqrt(distSq);
            Math::Vec3d dir = delta / dist;

            // Newton’s law of universal gravitation
            double force = (G * aBody.mass * bBody.mass) / distSq;

            // accelerations
            Math::Vec3f accA = dir * (force / aBody.mass);
            Math::Vec3f accB = -dir * (force / bBody.mass);

            aBody.acceleration += accA;
            bBody.acceleration += accB;

            // Check if A is tidally locked to B
            if (ECS::Get().HasComponent<TidallyLocked>(aID))
            {
                const auto& lockedEntityId = ECS::Get().GetComponent<TidallyLocked>(aID)->lockedEntity;
                if (lockedEntityId == bID)
                    ApplyTidalLock(aTransform, bTransform, aBody, cameraID);
            }

            // Check if B is tidally locked to A
            if (ECS::Get().HasComponent<TidallyLocked>(bID))
            {
                const auto& lockedEntityId = ECS::Get().GetComponent<TidallyLocked>(bID)->lockedEntity;
                if (lockedEntityId == aID)
                    ApplyTidalLock(bTransform, aTransform, bBody, cameraID);
            }
        }
    }
}

// Make Ta tidally locked towards Tb
void ApplyTidalLock(Transform& Ta, Transform& Tb, Rigidbody& Ra, EntityID& cameraID)
{
    Camera& camera = *ECS::Get().GetComponent<Camera>(cameraID);

    const Math::Vec3f& Pc = camera.GetPosition().GetWorld();

    const Math::Vec3f& Pa = Ta.position.GetWorld() - Pc;
    const Math::Vec3f& Pb = Tb.position.GetWorld() - Pc;

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
    Ra.angularVelocity.SetWorld(Math::Vec3f(0.0f));
}