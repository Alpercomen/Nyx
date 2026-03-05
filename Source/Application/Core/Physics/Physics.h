#pragma once
#include <iostream>

#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>

namespace Physics
{
    // Apply angular velocity to a transform quaternion
    void IntegrateAngularVelocity(Transform& tr, Rigidbody& rb, float64 dt)
    {
        Math::Vec3d w = rb.angularVelocity.GetWorld();
        float64 wlen = glm::length(w);
        if (wlen > 1e-8f)
        {
            Math::Vec3d axis = w / wlen;
            Math::Quatd dq = glm::angleAxis(wlen * dt, axis);
            tr.rotation.SetQuaternion(glm::normalize(dq * tr.rotation.GetQuaternion()));
        }
    }

    void Iterate(const EntityID& objID, float64 deltaTime)
    {
        const float64 dt = deltaTime * TIME_SCALE;

        Transform& transform = *ECS::Get().GetComponent<Transform>(objID);
        Rigidbody& rigidbody = *ECS::Get().GetComponent<Rigidbody>(objID);

        Position& pos = transform.position;
        Velocity& vel = rigidbody.velocity;

        Math::Vec3d next = pos.GetWorld() + vel.GetWorld() * deltaTime * TIME_SCALE;
        pos.SetWorld(next);

        // Apply rotation
        IntegrateAngularVelocity(transform, rigidbody, dt);
    }

	void Update(float64 deltaTime)
	{
        const auto& sphereIDs = ECS::Get().GetAllComponentIDs<Sphere>();

        for (size_t i = 0; i < sphereIDs.size(); ++i)
        {
            const EntityID& id = sphereIDs[i];
            if (!ECS::Get().HasComponent<Transform>(id) || !ECS::Get().HasComponent<Rigidbody>(id))
                continue;

            Attract(id);
            Iterate(id, deltaTime);
        }
	}
}