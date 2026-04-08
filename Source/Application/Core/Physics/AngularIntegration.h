#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>

namespace Nyx
{
    inline void IntegrateAngularVelocity(Transform& tr, Rigidbody& rb, float64 dt)
    {
        const Math::Vec3d w = rb.angularVelocity.GetWorld();
        const float64 wlen = glm::length(w);

        if (wlen <= 1e-12)
            return;

        const Math::Vec3d axis = w / wlen;
        const Math::Quatd dq = glm::angleAxis(wlen * dt, axis);

        tr.rotation.SetQuaternion(glm::normalize(dq * tr.rotation.GetQuaternion()));
    }

    inline void IntegrateTidallyLocked(const EntityID& id)
    {
        auto* tidalLockPtr = ECS::Get().GetComponent<TidallyLocked>(id);

        if (tidalLockPtr == nullptr)
            return;

        EntityID lockedId = tidalLockPtr->lockedEntity;

        if (!ECS::Get().HasComponent<Transform>(id) ||
            !ECS::Get().HasComponent<Rigidbody>(id) ||
            !ECS::Get().HasComponent<Transform>(lockedId) ||
            !ECS::Get().HasComponent<Rigidbody>(lockedId))
            return;

        auto& Ta = *ECS::Get().GetComponent<Transform>(id);
        auto& Ra = *ECS::Get().GetComponent<Rigidbody>(id);
        auto& Tb = *ECS::Get().GetComponent<Transform>(lockedId);
        auto& Rb = *ECS::Get().GetComponent<Rigidbody>(lockedId);

        ApplyTidalLock(Ta, Tb, Ra, Rb);
    }
}