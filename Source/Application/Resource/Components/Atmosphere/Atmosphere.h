#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    class AtmosphereComponent
    {
    public:
        Math::Vec3f color = { 0.393f, 0.564f, 0.968f };
        float32 intensity = 1.6f;
        float32 atmosphereRadius = 0.001f;
        float32 rimPower = 0.8f;
        float32 innerAmount = 0.2f;
        float32 lightSoftness = 0.5f;
        float32 alphaMultiplier = 1.0f;
    };
}