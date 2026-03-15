#pragma once

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Buffers/Vertex.h>

namespace Nyx
{
    class ModelImporter
    {
    public:
        static Model LoadFromFile(const String& filepath);
    };
}