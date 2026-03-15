#pragma once

#include <Application/Resource/Components/Components.h>
#include <Application/Services/Managers/ResourceManager/ResourceManager.h>

namespace Nyx
{
    class GridMesh {
    public:
        GridMesh();

        void DrawGrid(const Camera& camera, const Transform& cameraTransform) const;

    private:
        Vector<Math::Vec3f> m_vertices;
        VAO m_vao;
        Shader m_shader;
    };
}