#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    class AtmosphereComponent
    {
    public:
        Math::Vec3f color = { 0.455f, 0.602f, 1.000f };
        float32 radius = 0.025f;
        float32 haloIntensity = 1.505f;
        float32 haloAlpha = 0.358f;
        float32 rimStart = 0.989f;
        float32 rimEnd = 0.692f;
        float32 lightSoftness = 0.25f;
    };
}