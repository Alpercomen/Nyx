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

void InitializeCircularOrbit(EntityID satelliteID, EntityID attractorID, float32 inclination, bool isTidallyLocked)
{
    // Ensure required components exist
    if (!ECS::Get().HasComponent<Transform>(satelliteID) || !ECS::Get().HasComponent<Rigidbody>(satelliteID) ||
        !ECS::Get().HasComponent<Transform>(attractorID) || !ECS::Get().HasComponent<Rigidbody>(attractorID))
    {
        spdlog::error("Missing required components to initialize orbit.");
        return;
    }

    if (ECS::Get().HasComponent<Name>(satelliteID) && ECS::Get().HasComponent<Name>(attractorID))
    {
        const Name& satelliteName = *ECS::Get().GetComponent<Name>(satelliteID);
        const Name& attractorName = *ECS::Get().GetComponent<Name>(attractorID);

        spdlog::info("{} is set to orbit around {}", satelliteName.name, attractorName.name);
    }

    auto& satellitePos = ECS::Get().GetComponent<Transform>(satelliteID)->position;
    auto& attractorPos = ECS::Get().GetComponent<Transform>(attractorID)->position;

    auto& satelliteRig = *ECS::Get().GetComponent<Rigidbody>(satelliteID);
    auto& attractorRig = *ECS::Get().GetComponent<Rigidbody>(attractorID);

    Math::Vec3d r = satellitePos.GetWorld() - attractorPos.GetWorld();
    float64 radius = glm::length(r);

    if (radius <= 0.0)
    {
        spdlog::error("Cannot initialize orbit: satellite and attractor are at the same position.");
        return;
    }

    Math::Vec3d rHat = glm::normalize(r);
    Math::Vec3d orbitNormal(0.0, 1.0, 0.0);

    if (std::abs(inclination) > 0.0001f)
    {
        glm::dmat4 rot = glm::rotate(glm::dmat4(1.0), glm::radians(static_cast<float64>(inclination)), glm::dvec3(1.0, 0.0, 0.0));
        glm::dvec4 rotated = rot * glm::dvec4(orbitNormal.x, orbitNormal.y, orbitNormal.z, 0.0);
        orbitNormal = glm::normalize(Math::Vec3d(rotated.x, rotated.y, rotated.z));
    }

    Math::Vec3d tangent = glm::cross(orbitNormal, rHat);
    float64 tangentLength = glm::length(tangent);

    if (tangentLength <= 0.0)
    {
        spdlog::error("Cannot initialize orbit: tangent vector has zero length.");
        return;
    }

    tangent = tangent / tangentLength;

    const float64 mu = G * static_cast<float64>(attractorRig.mass);
    const float64 orbitalSpeed = std::sqrt(mu / radius);

    Math::Vec3d satelliteVel = attractorRig.velocity.GetWorld() + tangent * orbitalSpeed;
    satelliteRig.velocity.SetWorld(satelliteVel);

    spdlog::info("Initialized circular orbit:");
    spdlog::info(" - Radius: {:.3f} m", radius);
    spdlog::info(" - Inclination: {:.3f} deg", inclination);
    spdlog::info(" - Orbital speed: {:.6f} m/s", orbitalSpeed);
    spdlog::info(" - Relative tangent: ({:.6f}, {:.6f}, {:.6f})", tangent.x, tangent.y, tangent.z);
    spdlog::info(" - Satellite world velocity: ({:.6f}, {:.6f}, {:.6f})", satelliteVel.x, satelliteVel.y, satelliteVel.z);
}

void Attract(const EntityID& objID)
{
	if (!ECS::Get().HasComponent<Rigidbody>(objID) || !ECS::Get().HasComponent<Transform>(objID))
		return;

    auto sphereIDs = ECS::Get().GetAllComponentIDs<Sphere>();

	for (EntityID id : ECS::Get().View<Transform, Rigidbody>())
	{
		if (objID == id)
			continue;

        auto& objTransform  = *ECS::Get().GetComponent<Transform>(objID);
        auto& objRigidbody  = *ECS::Get().GetComponent<Rigidbody>(objID);

		auto& obj2Transform  = *ECS::Get().GetComponent<Transform>(id);
		auto& obj2Rigidbody  = *ECS::Get().GetComponent<Rigidbody>(id);

		double dx = objTransform.position.GetWorld().x - obj2Transform.position.GetWorld().x;
		double dy = objTransform.position.GetWorld().y - obj2Transform.position.GetWorld().y;
		double dz = objTransform.position.GetWorld().z - obj2Transform.position.GetWorld().z;

		Math::Vec3d diff = Math::Vec3d(dx, dy, dz);
		float distance = glm::length(diff);
		Math::Vec3d unitVector = glm::normalize(diff);

        // F = ( G * m1 * m2 ) / d^2
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

        Math::Vec3d vel = obj2Rigidbody.velocity.GetWorld();
	}
}

// Make Ta tidally locked towards Tb
void ApplyTidalLock(Transform& Ta, Transform& Tb, Rigidbody& Ra)
{
    const Math::Vec3d& Pa = Ta.position.GetWorld();
    const Math::Vec3d& Pb = Tb.position.GetWorld();

    Math::Vec3d dir = Pb - Pa;
    if (glm::length2(dir) < 1e-12f)
        return;

    dir = glm::normalize(dir);

    Math::Vec3d up = Math::Vec3d(0, 1, 0);
    if (glm::abs(glm::dot(up, dir)) > 0.99f)
        up = Math::Vec3d(1, 0, 0);

    const Math::Vec3d x = -dir;
    const Math::Vec3d z = -glm::normalize(glm::cross(up, x));
    const Math::Vec3d y = glm::cross(z, x);

    const Math::Mat3f basis(x, y, z);
    Math::Quatf qWorld = glm::normalize(glm::quat_cast(basis));

    Ta.rotation.SetQuaternion(qWorld);
    Ra.angularVelocity.SetWorld(Math::Vec3d(0.0f));
}

void ComputeStrongestAttractors(Vector<SimBody>& bodies)
{
    for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
        bodies[i].strongestAttractorID = ComputeStrongestAttractorID(bodies, i);
}

void ComputeSOIRadii(Vector<SimBody>& bodies)
{
    for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
    {
        SimBody& body = bodies[i];
        body.soiRadius = 0.0;

        if (!body.active || body.mass <= 0.0)
            continue;

        if (body.strongestAttractorID == NO_ID)
            continue;

        int32 parentIndex = -1;
        for (int32 j = 0; j < static_cast<int32>(bodies.size()); ++j)
        {
            if (bodies[j].id == body.strongestAttractorID)
            {
                parentIndex = j;
                break;
            }
        }

        if (parentIndex < 0)
            continue;

        const SimBody& parent = bodies[parentIndex];
        if (!parent.active || parent.mass <= 0.0)
            continue;

        const float64 a = glm::length(body.position - parent.position);
        if (a <= 1e-6)
            continue;

        body.soiRadius = a * std::pow(body.mass / parent.mass, 2.0 / 5.0);
    }
}

EntityID ComputeStrongestAttractorID(const Vector<SimBody>& bodies, int32 selfIndex)
{
    const SimBody& self = bodies[selfIndex];

    EntityID bestID = NO_ID;
    float64 bestAccelSq = -1.0;

    if (!self.active || self.mass <= 0.0)
        return NO_ID;

    for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
    {
        if (i == selfIndex || !bodies[i].active || bodies[i].mass <= 0.0)
            continue;

        const SimBody& other = bodies[i];

        Math::Vec3d r = other.position - self.position;
        float64 distSq = glm::dot(r, r) + other.softening * other.softening;

        if (distSq < 1e-18)
            continue;

        float64 invDist = 1.0 / std::sqrt(distSq);
        float64 invDist3 = invDist * invDist * invDist;

        Math::Vec3d accel = r * (G * other.mass * invDist3);
        float64 accelSq = glm::dot(accel, accel);

        if (accelSq > bestAccelSq)
        {
            bestAccelSq = accelSq;
            bestID = other.id;
        }
    }

    return bestID;
}

EntityID ComputeOrbitalParentID(const Vector<SimBody>& bodies, int32 selfIndex)
{
    const SimBody& self = bodies[selfIndex];

    EntityID bestID = NO_ID;
    float64 bestDistance = std::numeric_limits<float64>::infinity();

    for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
    {
        if (i == selfIndex || !bodies[i].active || bodies[i].mass <= 0.0)
            continue;

        const SimBody& candidate = bodies[i];

        if (candidate.mass <= self.mass)
            continue;

        if (candidate.soiRadius <= 0.0)
            continue;

        const float64 r = glm::length(self.position - candidate.position);

        if (r < candidate.soiRadius && r < bestDistance)
        {
            bestDistance = r;
            bestID = candidate.id;
        }
    }

    return bestID;
}