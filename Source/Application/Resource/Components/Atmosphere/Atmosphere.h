#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    class AtmosphereComponent
    {
    public:
        Math::Vec3f color = { 0.631f, 0.731f, 1.000f };
        float32 radius = 0.010f;
        float32 haloIntensity = 1.040f;
        float32 haloAlpha = 0.645f;
        float32 rimStart = 0.939f;
        float32 rimEnd = 0.829f;
        float32 lightSoftness = 0.25f;
    };
}