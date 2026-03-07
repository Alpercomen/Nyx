#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
    class AtmosphereComponent
    {
    public:
        Math::Vec3f color = { 0.640f, 0.796f, 1.000f };
        float32 thickness = 0.003f;
        float32 intensity = 1.4f;
        float32 rimPower = 0.75f;
        float32 lightPower = 0.6f;
    };
}