#pragma once

#include <Application/Core/Core.h>
#include <Application/Utils/ShaderUtils/ShaderUtils.h>

#include <Application/Core/Services/Managers/RenderManager/OpenGL.h>

namespace Nyx
{
    class Shader
    {
    public:
        Shader()
            : id(NO_ID)
        {
        }

        // Store the paths; do not compile yet.
        Shader(const String& vertexPath, const String& fragmentPath)
            : id(NO_ID), vsPath(vertexPath), fsPath(fragmentPath)
        {
        }

        // First call compiles/links; subsequent calls just bind.
        void Use()
        {
            if (id == NO_ID)
            {
                id = CreateShaderProgram(vsPath, fsPath);
            }
            GL::Get()->glUseProgram(id);
        }

        uint32 GetID() const { return id; }

    private:
        uint32 id;
        String vsPath;
        String fsPath;
    };
}