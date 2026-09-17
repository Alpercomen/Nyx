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

void InitializeCircularOrbit(EntityID satelliteID, EntityID attractorID, float32 inclination)
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

void InitializeOrbitFromApsides(
    EntityID satelliteID, 
    EntityID attractorID, 
    float64 periapsis, 
    float64 apoapsis, 
    float64 inclination, 
    float64 longitudeAscendingNode, 
    float64 argumentOfPeriapsis, 
    bool startAtPeriapsis)
{
    if (periapsis <= 0.0 || apoapsis <= 0.0 || apoapsis < periapsis)
    {
        spdlog::error("Invalid orbit apsides: " "periapsis must be > 0 and apoapsis >= periapsis.");

        return;
    }

    const float64 semiMajorAxis = (periapsis + apoapsis) * 0.5;
    const float64 eccentricity = (apoapsis - periapsis) / (apoapsis + periapsis);

    OrbitalElements elements;

    elements.semiMajorAxis = semiMajorAxis;
    elements.eccentricity = eccentricity;
    elements.inclination = inclination;
    elements.longitudeAscendingNode = longitudeAscendingNode;
    elements.argumentOfPeriapsis = argumentOfPeriapsis;
    elements.trueAnomaly = startAtPeriapsis ? 0.0 : 180.0;

    InitializeOrbit(satelliteID, attractorID, elements);
}

void InitializeOrbit(EntityID satelliteID, EntityID attractorID, const OrbitalElements& elements)
{
    auto& ecs = ECS::Get();

    if (!ecs.HasComponent<Transform>(satelliteID) || !ecs.HasComponent<Rigidbody>(satelliteID) ||
        !ecs.HasComponent<Transform>(attractorID) || !ecs.HasComponent<Rigidbody>(attractorID))
    {
        spdlog::error("Cannot initialize orbit: missing required components.");

        return;
    }

    auto* satelliteTransform = ecs.GetComponent<Transform>(satelliteID);
    auto* attractorTransform = ecs.GetComponent<Transform>(attractorID);
    auto* satelliteRigidbody = ecs.GetComponent<Rigidbody>(satelliteID);
    auto* attractorRigidbody = ecs.GetComponent<Rigidbody>(attractorID);

    if (elements.semiMajorAxis <= 0.0)
    {
        spdlog::error("Cannot initialize orbit: semi-major axis must be > 0.");
        return;
    }

    if (elements.eccentricity < 0.0 || elements.eccentricity >= 1.0)
    {
        spdlog::error("Cannot initialize orbit: eccentricity must satisfy ""0 <= e < 1 for an elliptical orbit.");
        return;
    }

    const float64 mu = G * static_cast<float64>(attractorRigidbody->mass);

    if (mu <= 0.0)
    {
        spdlog::error("Cannot initialize orbit: attractor has invalid mass.");
        return;
    }

    const float64 a = elements.semiMajorAxis;
    const float64 e = elements.eccentricity;
    const float64 nu = glm::radians(elements.trueAnomaly);

    const float64 p = a * (1.0 - e * e); // Semi-latus rectum

    if (p <= 0.0)
    {
        spdlog::error("Cannot initialize orbit: invalid semi-latus rectum.");
        return;
    }

    const float64 cosNu = std::cos(nu);
    const float64 sinNu = std::sin(nu);

    const float64 denominator = 1.0 + e * cosNu;

    if (std::abs(denominator) < 1e-12)
    {
        spdlog::error("Cannot initialize orbit: invalid true anomaly.");
        return;
    }

    const float64 radius = p / denominator;
    Math::Vec3d positionPQW(radius * cosNu, 0.0, radius * sinNu);

    const float64 velocityScale = std::sqrt(mu / p);
    Math::Vec3d velocityPQW(-velocityScale * sinNu, 0.0, velocityScale * (e + cosNu));

    const float64 inclination = glm::radians(elements.inclination);
    const float64 ascendingNode = glm::radians(elements.longitudeAscendingNode);
    const float64 argumentPeriapsis = glm::radians(elements.argumentOfPeriapsis);

    const float64 Omega = glm::radians(elements.longitudeAscendingNode);
    const float64 inc = glm::radians(elements.inclination);
    const float64 omega = glm::radians(elements.argumentOfPeriapsis);

    const float64 cosOmega = std::cos(Omega);
    const float64 sinOmega = std::sin(Omega);

    const float64 cosInc = std::cos(inc);
    const float64 sinInc = std::sin(inc);

    const float64 cosOmegaArg = std::cos(omega);
    const float64 sinOmegaArg = std::sin(omega);

    // Periapsis direction (P)
    Math::Vec3d P(cosOmega * cosOmegaArg - sinOmega * sinOmegaArg * cosInc, sinOmegaArg * sinInc, sinOmega * cosOmegaArg + cosOmega * sinOmegaArg * cosInc);

    // In-plane direction 90 degrees from periapsis (Q)
    Math::Vec3d Q(cosOmega * sinOmegaArg + sinOmega * cosOmegaArg * cosInc, -cosOmegaArg * sinInc, sinOmega * sinOmegaArg - cosOmega * cosOmegaArg * cosInc);

    // Orbital normal
    Math::Vec3d W = glm::normalize(glm::cross(P, Q));

    Math::Vec3d relativePosition = P * positionPQW.x + Q * positionPQW.z;
    Math::Vec3d relativeVelocity = P * velocityPQW.x + Q * velocityPQW.z;

    Math::Vec3d attractorPosition = attractorTransform->position.GetWorld();
    Math::Vec3d attractorVelocity = attractorRigidbody->velocity.GetWorld();

    Math::Vec3d satellitePosition = attractorPosition + relativePosition;
    Math::Vec3d satelliteVelocity = attractorVelocity + relativeVelocity;

    satelliteTransform->position.SetWorld(satellitePosition);
    satelliteRigidbody->velocity.SetWorld(satelliteVelocity);

    const float64 periapsis = a * (1.0 - e);
    const float64 apoapsis = a * (1.0 + e);

    const float64 orbitalPeriod = 2.0 * glm::pi<float64>() * std::sqrt((a * a * a) / mu);

    spdlog::info("Initialized Keplerian orbit:");
    spdlog::info(" - Semi-major axis: {:.3f} m", a);
    spdlog::info(" - Eccentricity: {:.6f}", e);
    spdlog::info(" - Periapsis: {:.3f} m", periapsis);
    spdlog::info(" - Apoapsis: {:.3f} m", apoapsis);
    spdlog::info(" - Inclination: {:.3f} deg", elements.inclination);
    spdlog::info(" - Longitude of ascending node: {:.3f} deg", elements.longitudeAscendingNode);
    spdlog::info(" - Argument of periapsis: {:.3f} deg", elements.argumentOfPeriapsis);
    spdlog::info(" - True anomaly: {:.3f} deg", elements.trueAnomaly);
    spdlog::info(" - Current radius: {:.3f} m", radius);
    spdlog::info(" - Orbital period: {:.3f} s", orbitalPeriod);
    spdlog::info(" - Relative position: ({:.3f}, {:.3f}, {:.3f})", relativePosition.x, relativePosition.y, relativePosition.z);
    spdlog::info(" - Relative velocity: ({:.6f}, {:.6f}, {:.6f})", relativeVelocity.x, relativeVelocity.y, relativeVelocity.z);
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

        Math::Vec3d vel = obj2Rigidbody.velocity.GetWorld();
	}
}

// Make Ta tidally locked towards Tb
void ApplyTidalLock(Transform& Ta, Transform& Tb, Rigidbody& Ra, Rigidbody& Rb)
{
    const Math::Vec3d& Pa = Ta.position.GetWorld();
    const Math::Vec3d& Pb = Tb.position.GetWorld();

    Math::Vec3d dirToTarget = Pb - Pa;
    if (glm::length2(dirToTarget) < 1e-12)
        return;

    dirToTarget = glm::normalize(dirToTarget);

    Math::Vec3d y = -dirToTarget;

    // Tangential relative velocity direction
    Math::Vec3d vel = Rb.velocity.GetWorld() - Ra.velocity.GetWorld();
    Math::Vec3d z = vel - glm::dot(vel, y) * y;

    if (glm::length2(z) < 1e-12)
    {
        Math::Vec3d fallback(0.0, 0.0, 1.0);
        if (glm::abs(glm::dot(fallback, y)) > 0.99)
            fallback = Math::Vec3d(1.0, 0.0, 0.0);

        z = fallback - glm::dot(fallback, y) * y;
    }

    z = -glm::normalize(z);

    Math::Vec3d x = glm::normalize(glm::cross(y, z));
    z = glm::normalize(glm::cross(x, y));

    const Math::Mat3f basis(x, y, z);
    Math::Quatf qWorld = glm::normalize(glm::quat_cast(basis));

    Ta.rotation.SetQuaternion(qWorld);
    Ra.angularVelocity.SetWorld(Math::Vec3d(0.0));
}

void ApplyTidalLockWithVelocityFacing(Transform& Ta, Transform& Tb, Rigidbody& Ra)
{
    const Math::Vec3d& Pa = Ta.position.GetWorld();
    const Math::Vec3d& Pb = Tb.position.GetWorld();

    Math::Vec3d dirToTarget = Pb - Pa;
    if (glm::length2(dirToTarget) < 1e-12)
        return;

    dirToTarget = glm::normalize(dirToTarget);

    Math::Vec3d y = -dirToTarget;
    Math::Vec3d vel = Ra.velocity.GetWorld();
    Math::Vec3d z = vel - glm::dot(vel, y) * y;

    if (glm::length2(z) < 1e-12)
    {
        Math::Vec3d fallback = Math::Vec3d(0, 0, 1);

        if (glm::abs(glm::dot(fallback, y)) > 0.99)
            fallback = Math::Vec3d(1, 0, 0);

        z = fallback - glm::dot(fallback, y) * y;
    }

    z = glm::normalize(z);

    Math::Vec3d x = glm::normalize(glm::cross(y, z));
    z = glm::normalize(glm::cross(x, y));

    const Math::Mat3f basis(x, y, z);
    Math::Quatf qWorld = glm::normalize(glm::quat_cast(basis));

    Ta.rotation.SetQuaternion(qWorld);
    Ra.angularVelocity.SetWorld(Math::Vec3d(0.0));
}

void ComputeStrongestAttractors(Vector<SimBody>& bodies)
{
    for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
        bodies[i].strongestAttractorID = ComputeStrongestAttractorID(bodies, i);
}

void ComputeTimeStepParents(Vector<SimBody>& bodies)
{
    for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
        bodies[i].timestepParentID = ComputeTimeStepParentID(bodies, i);
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

EntityID ComputeTimeStepParentID(const Vector<SimBody>& bodies, int32 selfIndex)
{
    const SimBody& self = bodies[selfIndex];

    if (!self.active || self.mass <= 0.0)
        return NO_ID;

    EntityID bestID = NO_ID;
    float64 bestTimeScale = std::numeric_limits<float64>::infinity();

    for (int32 i = 0; i < static_cast<int32>(bodies.size()); ++i)
    {
        if (i == selfIndex || !bodies[i].active || bodies[i].mass <= 0.0)
            continue;

        const SimBody& candidate = bodies[i];

        if (candidate.mass <= self.mass)
            continue;

        const float64 r = glm::length(self.position - candidate.position);
        if (r <= 1e-6)
            continue;

        const float64 mu = G * candidate.mass;
        const float64 timeScale = std::sqrt((r * r * r) / mu);

        if (timeScale < bestTimeScale)
        {
            bestTimeScale = timeScale;
            bestID = candidate.id;
        }
    }

    return bestID;
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

    if (bestID != NO_ID)
        return bestID;

    return self.strongestAttractorID;
}