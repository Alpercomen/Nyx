#pragma once
#include <iostream>

#include <Application/Constants/Constants.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

namespace Physics
{
    void Iterate(const EntityID& objID, float deltaTime)
    {
        auto& transform = *ECS::Get().GetComponent<Transform>(objID);
        auto& rigidbody = *ECS::Get().GetComponent<Rigidbody>(objID);

        auto& pos = transform.position;
        auto& vel = rigidbody.velocity;

        glm::vec3 next = pos.GetWorld() + vel.GetWorld() * deltaTime * TIME_SCALE;
        pos.SetWorld(next);
    }

	void Update(float deltaTime)
	{
        const auto& sphereIDs = ECS::Get().GetAllComponentIDs<Sphere>();

        for (size_t i = 0; i < sphereIDs.size(); ++i)
        {
            const EntityID& id = sphereIDs[i];
            if (!ECS::Get().HasComponent<Transform>(id))
                continue;

            Rotate(id, deltaTime);

            if (!ECS::Get().HasComponent<Rigidbody>(id))
                continue;

            Attract(id);
            Iterate(id, deltaTime);
        }
	}
}