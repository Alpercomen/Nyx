#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    class AtmosphereComponent
    {
    public:
        Math::Vec3f color = { 0.119f, 0.305f, 0.977f };
        float32 radius = 0.014f;
        float32 haloIntensity = 2.925f;
        float32 haloAlpha = 0.449f;
        float32 rimStart = 1.014f;
        float32 rimEnd = 0.72f;
        float32 lightSoftness = 0.25f;
    };
}