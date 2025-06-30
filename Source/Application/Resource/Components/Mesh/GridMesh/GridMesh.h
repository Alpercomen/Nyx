#pragma once

#include <Application/Resource/Components/Mesh/Mesh.h>
#include <Application/Resource/ShaderProgram/ShaderProgram.h>
#include <Application/Resource/Components/Components.h>

#include <Application/Core/Pipeline/Immediate/Immediate.h>

class GridMesh {
public:
    GridMesh();

    void DrawGrid(const Camera& camera) const;

private:
    Vector<Math::Vec3f> m_vertices;
    Mesh m_mesh;
    Shader m_shader;
};