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

    if (std::abs(glm::dot(direction, up)) > 0.99f) {
        up = glm::vec3(1, 0, 0); // avoid near-parallel vectors
    }

    glm::vec3 tangential = glm::normalize(glm::cross(direction, up));
    glm::vec3 satelliteVel = tangential * static_cast<float>(orbitalSpeed);

    // Apply to satellite
    satelliteRig.velocity.SetWorld(satelliteVel);

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

        // Apply rotational torque caused by the attraction of two bodies around the barycenter
        ApplyRotationalTorque(objID, id);
	}
}

// Try to approximate scalar moment of inertia I. If we can get a radius from
// a Sphere component, use solid sphere I = (2/5) m R^2. Otherwise fall back
// to a mass-weighted proxy (bigger mass -> larger inertia).
static double ApproxMomentOfInertia(EntityID id, const Rigidbody& rb)
{
    double m = rb.mass;
    return std::max(1e-6, 0.4 * m); // 0.4 mimics solid sphere factor without R^2
}

// Returns angular acceleration vector for “align forward to dirTarget”
static glm::vec3 AlignmentAngularAcceleration(const glm::vec3& forward, const glm::vec3& dirTarget, double alignGain, double I, double maxAngAcc) 
{
    glm::vec3 f = glm::normalize(forward);
    glm::vec3 d = glm::normalize(dirTarget);

    // Axis that would rotate f toward d
    glm::vec3 axis = glm::cross(f, d);
    double misalign = static_cast<double>(glm::length(axis));
    if (misalign < 1e-8) return glm::vec3(0);

    axis = glm::normalize(axis);

    // Simple physics-inspired model: torque ~ gain * misalignment,
    // angular acceleration = torque / I
    double aMag = (alignGain * misalign) / std::max(1e-12, I);

    // Clamp for stability
    aMag = std::min(aMag, static_cast<double>(maxAngAcc));
    return static_cast<float>(aMag) * axis;
}

// Apply angular velocity to a transform quaternion
static void IntegrateAngularVelocity(Transform& tr, Rigidbody& rb, float dt)
{
    glm::vec3 w = rb.angularVelocity.GetWorld();
    float wlen = glm::length(w);
    if (wlen > 1e-8f) 
    {
        glm::vec3 axis = w / wlen;
        glm::quat dq = glm::angleAxis(wlen * dt, axis);
        tr.rotation.SetQuaternion(glm::normalize(dq * tr.rotation.GetQuaternion()));
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

// --- main tidal torque -----------------------------------------------------
void ApplyRotationalTorque(EntityID aID, EntityID bID)
{
    if (!ECS::Get().HasComponent<Transform>(aID) ||
        !ECS::Get().HasComponent<Transform>(bID) ||
        !ECS::Get().HasComponent<Rigidbody>(aID) ||
        !ECS::Get().HasComponent<Rigidbody>(bID)) 
    {
        return;
    }

    auto& Ta = *ECS::Get().GetComponent<Transform>(aID);
    auto& Tb = *ECS::Get().GetComponent<Transform>(bID);
    auto& Ra = *ECS::Get().GetComponent<Rigidbody>(aID);
    auto& Rb = *ECS::Get().GetComponent<Rigidbody>(bID);

    // If Ra is tidally locked to Rb
    if (ECS::Get().HasComponent<TidallyLocked>(aID))
    {
        const auto& lockedEntityId = ECS::Get().GetComponent<TidallyLocked>(aID)->lockedEntity;

        if (lockedEntityId == bID)
            ApplyTidalLock(Ta, Tb, Ra);

        return;
    }

    const glm::vec3 pa = Ta.position.GetWorld();
    const glm::vec3 pb = Tb.position.GetWorld();
    glm::vec3 rab = pb - pa;
    float r = glm::length(rab);

    if (r < 1e-6f) 
        return;

    glm::vec3 dirAB = rab / r;        // a -> b

    // Time step
    const float dt = DELTA_TIME * TIME_SCALE;

    // Inertia estimates
    const double Ia = ApproxMomentOfInertia(aID, Ra);

    // Optional: make alignment stronger when gravity is stronger (1/r^2),
    // but keep it small for stability. You can tune baseGain.
    const double baseGain = 1e-4;   // try 1e-6 .. 1e-4 depending on your scale
    const double gravScale = (G * Ra.mass * Rb.mass) / std::max(1e-6f, r * r);
    const double gainA = baseGain * gravScale;   // torque tendency on A

    // Clamp on angular acceleration (rad/s^2)
    const double maxAngAcc = 5e+1; // tighten/loosen if needed

    // Desired facing vector: use each body's current “forward”
    const glm::vec3 fA = Ta.rotation.GetForward();

    // Angular accelerations
    glm::vec3 alphaA = AlignmentAngularAcceleration(fA, dirAB, gainA, Ia, maxAngAcc);

    // Integrate angular velocity
    Ra.angularVelocity.SetWorld(Ra.angularVelocity.GetWorld() + alphaA * dt);

    // Damping (critically important for stability at large time scales)
    const float dampingPerSec = 0.5f; // 0.5 -> strong damping; tune as needed
    const float damper = glm::clamp(1.0f - dampingPerSec * dt, 0.90f, 0.9999f);
    Ra.angularVelocity.SetWorld(Ra.angularVelocity.GetWorld() * damper);

    // Apply rotation
    IntegrateAngularVelocity(Ta, Ra, dt);
}