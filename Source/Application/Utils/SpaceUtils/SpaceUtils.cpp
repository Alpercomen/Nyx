#pragma once
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Constants/Constants.h>


double GravitationalForce(double mu, double r) 
{
	return mu / (r * r);
}

double CalculateOrbitalVelocity(double otherMass, double r)
{
	return std::sqrt(G * otherMass / r) / METER_PER_KILOMETER;
}

void InitializeCircularOrbit(EntityID satelliteID, EntityID attractorID) {
    // Ensure required components
    if (!ECS::Get().HasComponent<Transform>(satelliteID) ||
        !ECS::Get().HasComponent<Transform>(attractorID) ||
        !ECS::Get().HasComponent<Rigidbody>(satelliteID) ||
        !ECS::Get().HasComponent<Rigidbody>(attractorID)) 
    {
        spdlog::error("Missing required components to initialize orbit.");
        return;
    }

    auto& satellitePos = ECS::Get().GetComponent<Transform>(satelliteID)->position;
    auto& attractorPos = ECS::Get().GetComponent<Transform>(attractorID)->position;

    auto& satelliteRig = *ECS::Get().GetComponent<Rigidbody>(satelliteID);
    auto& attractorRig = *ECS::Get().GetComponent<Rigidbody>(attractorID);

    glm::vec3 direction = glm::normalize(attractorPos.GetWorld() - satellitePos.GetWorld());
    float distanceKm = glm::length(attractorPos.GetWorld() - satellitePos.GetWorld());

    double orbitalSpeed = CalculateOrbitalVelocity(attractorRig.mass, distanceKm * 2);

    // Compute tangential direction
    glm::vec3 up = glm::vec3(0, 1, 0);

    // If direction is nearly parallel with up, pick alternate
    if (std::abs(glm::dot(direction, up)) > 0.99f)
    {
        up = glm::vec3(1, 0, 0);
    }

    glm::vec3 tangential = glm::normalize(glm::cross(direction, up));
    glm::vec3 satelliteVel = tangential * static_cast<float>(orbitalSpeed);

    // Apply to satellite
    if (ECS::Get().HasComponent<Rigidbody>(satelliteID))
    {
        Rigidbody& rigidbody = *ECS::Get().GetComponent<Rigidbody>(satelliteID);

        rigidbody.velocity.SetWorld(rigidbody.velocity.GetWorld() + satelliteVel);
    } 

    // Conservation of momentum: Apply opposite to attractor
    glm::vec3 momentum = satelliteVel * static_cast<float>(satelliteRig.mass);
    glm::vec3 attractorDeltaVel = -momentum / static_cast<float>(attractorRig.mass);

    if (ECS::Get().HasComponent<Rigidbody>(attractorID))
    {
        Rigidbody& rigidbody = *ECS::Get().GetComponent<Rigidbody>(attractorID);

        rigidbody.velocity.SetWorld(rigidbody.velocity.GetWorld() + attractorDeltaVel);
    } 

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
		float distance = glm::length(diff) * METER_PER_KILOMETER;
		Math::Vec3d unitVector = glm::normalize(diff);

		float Gforce = (G * objRigidbody.mass * obj2Rigidbody.mass) / (distance * distance);
		float acc = Gforce / obj2Rigidbody.mass;

        Math::Vec3d accVec(acc * unitVector.x, acc * unitVector.y, acc * unitVector.z);
		Acceleration attraction(accVec);

        obj2Rigidbody.acceleration = attraction;
        obj2Rigidbody.velocity.Accelerate(attraction);
	}
}