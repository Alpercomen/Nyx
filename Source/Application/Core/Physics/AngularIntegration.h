#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

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
}