#pragma once
#include <iostream>

#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

namespace Physics
{
	void Update(float deltaTime)
	{
        const auto& sphereIDs = ECS::Get().GetAllComponentIDs<Sphere>();

        for (size_t i = 0; i < sphereIDs.size(); ++i)
        {
            const EntityID& id = sphereIDs[i];
            if (!ECS::Get().HasComponent<Transform>(id) || !ECS::Get().HasComponent<Rigidbody>(id))
                continue;

            Attract(id);

            auto& transform = *ECS::Get().GetComponent<Transform>(id);
            auto& rigidbody = *ECS::Get().GetComponent<Rigidbody>(id);

            auto& pos = transform.position;
            auto& vel = rigidbody.velocity;

            glm::vec3 next = pos.GetWorld() + vel.GetWorld() * deltaTime * TIME_SCALE;
            pos.SetWorld(next);
        }
	}
}