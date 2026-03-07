#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    class AtmosphereComponent
    {
    public:
        Math::Vec3f color = { 0.387f, 0.615f, 0.914f };
        float32 thickness = 0.006f;
        float32 intensity = 1.7f;
        float32 rimPower = 0.75f;
        float32 lightPower = 0.6f;
    };
}