#pragma once
#include <iostream>

#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Core/Services/Managers/SceneManager/SceneManager.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>

using namespace Nyx;

namespace Physics
{
    // Apply angular velocity to a transform quaternion
    void IntegrateAngularVelocity(Transform& tr, Rigidbody& rb, float dt)
    {
        Math::Vec3f w = rb.angularVelocity.GetWorld();
        float wlen = glm::length(w);
        if (wlen > 0.0f)
        {
            Math::Vec3f axis = w / wlen;
            glm::quat dq = glm::angleAxis(wlen * dt, axis);
            tr.rotation.SetQuaternion(glm::normalize(dq * tr.rotation.GetQuaternion()));
        }
    }

    void Integrate(float deltaTime)
    {
        const float dt = deltaTime * TIME_SCALE;

        auto& ids = ECS::Get().GetAllComponentIDs<Rigidbody>();
        for (const EntityID& id : ids)
        {
            if (!ECS::Get().HasComponent<Rigidbody>(id) || !ECS::Get().HasComponent<Transform>(id))
                continue;

            auto& rb = *ECS::Get().GetComponent<Rigidbody>(id);
            auto& tr = *ECS::Get().GetComponent<Transform>(id);

            Math::Vec3f vel = rb.velocity.GetWorld();
            Math::Vec3f pos = tr.position.GetWorld();

            vel += rb.acceleration.GetWorld() * dt;
            pos += vel * dt;

            rb.velocity.SetWorld(vel);
            tr.position.SetWorld(pos);

            IntegrateAngularVelocity(tr, rb, dt);
        }
    }

	void Update(EntityID cameraID)
	{
        Attract(cameraID);
        Integrate(DELTA_TIME);
	}
}