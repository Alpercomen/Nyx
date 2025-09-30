#pragma once

#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Components/Mesh/Mesh.h>
#include <Application/Resource/Material/ShaderProgram/ShaderProgram.h>

#include <Application/Core/Services/ResourceLocator/ResourceLocator.h>
#include <Application/Core/Services/Pipeline/Immediate/Immediate.h>
#include <Application/Core/Services/Managers/ResourceManager/ResourceManager.h>

class GridMesh {
public:
    GridMesh();

    void DrawGrid(const Camera& camera);

private:
    Vector<Math::Vec3f> m_vertices;
    Mesh m_mesh;
    Shader m_shader;
};