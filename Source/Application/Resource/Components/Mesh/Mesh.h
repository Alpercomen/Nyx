#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Application/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Services/Managers/ResourceManager/ResourceManager.h>
#include <Application/Services/Pipeline/Immediate/Immediate.h>
#include <Application/Services/ResourceLocator/ResourceLocator.h>
#include <Application/Services/Lighting/LightingSystem.h>

#include <Application/Resource/Components/Transform/Position.h>
#include <Application/Resource/Components/Rigidbody/Velocity.h>
#include <Application/Resource/Components/Rigidbody/Acceleration.h>
#include <Application/Resource/Components/Camera/Camera.h>
#include <Application/Resource/Components/Material/Material.h>
#include <Application/Resource/Buffers/VAO.h>
#include <Application/Resource/Buffers/VBO.h>
#include <Application/Resource/Buffers/EBO.h>
#include <Application/Resource/Buffers/Vertex.h>
#include <Application/Core/Physics/Meter.h>

namespace Nyx
{
    struct Mesh
    {
        VAO vao;
        VBO vbo;
        EBO ebo;
    };

    struct MeshSection
    {
        uint32 indexOffset = 0;
        uint32 indexCount = 0;
        uint32 materialIndex = 0;
    };

    struct ModelMaterial
    {
        Texture* texture = nullptr;
        Math::Vec3f baseColor = { 1.0f, 1.0f, 1.0f };
    };

    class Model
    {
    public:
        Model()
        {
            Shader shader = ResourceManager::GetShader(
                "ModelShader",
                R"(Nyx\Source\Application\Shaders\Model\model.vert)",
                R"(Nyx\Source\Application\Shaders\Model\model.frag)"
            );

            m_material = MakeShared<Material>(shader);
        }

        ~Model() = default;

        void UploadToGpu()
        {
            if (m_vertices.empty() || m_indices.empty())
                return;

            if (!m_mesh)
                m_mesh = MakeShared<Mesh>();

            glGenVertexArrays(1, &m_mesh->vao.m_data);
            glBindVertexArray(m_mesh->vao.m_data);

            glGenBuffers(1, &m_mesh->vbo.m_data);
            glBindBuffer(GL_ARRAY_BUFFER, m_mesh->vbo.m_data);
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &m_mesh->ebo.m_data);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh->ebo.m_data);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,m_indices.size() * sizeof(uint32),m_indices.data(),GL_STATIC_DRAW);

            m_mesh->ebo.m_indexCount = static_cast<uint32>(m_indices.size());
            constexpr GLsizei stride = sizeof(Vertex);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, position));

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, tangent));

            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, texCoord));
        }

        void DrawModel(Math::Mat4f& model, Math::Mat4f& view, Math::Mat4f& projection)
        {
            if (!m_material)
                return;

            m_material->Bind();
            
            uint32 shaderID = m_material->GetShader().GetID();
            Math::Vec3f color(1.0f, 1.0f, 1.0f);

            GLuint modelLoc = glGetUniformLocation(shaderID, "uModel");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            GLuint viewLoc = glGetUniformLocation(shaderID, "uView");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            GLuint projLoc = glGetUniformLocation(shaderID, "uProj"); 
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            GLuint baseColorLoc = glGetUniformLocation(shaderID, "uBaseColor");
            glUniform3fv(baseColorLoc, 1, glm::value_ptr(color));

            glBindVertexArray(m_mesh->vao.m_data);

            ImmediatePipeline::Get().Begin();
            ImmediatePipeline::Get().UseModel();
            
            for (const auto& section : m_sections)
            {
                const ModelMaterial* mat = nullptr;

                if (section.materialIndex < m_materials.size())
                    mat = &m_materials[section.materialIndex];

                Math::Vec3f baseColor = { 1.0f, 1.0f, 1.0f };
                bool useTexture = false;

                if (mat)
                {
                    baseColor = mat->baseColor;

                    if (mat->texture)
                    {
                        mat->texture->Bind(0);
                        useTexture = true;
                    }
                }

                GLuint baseColorLoc = glGetUniformLocation(shaderID, "uBaseColor");
                glUniform3fv(baseColorLoc, 1, glm::value_ptr(baseColor));

                GLuint hasTextureLoc = glGetUniformLocation(shaderID, "uHasTexture");
                glUniform1i(hasTextureLoc, useTexture ? 1 : 0);

                if (useTexture)
                {
                    GLuint textureLoc = glGetUniformLocation(shaderID, "uTexture");
                    glUniform1i(textureLoc, 0);
                }

                glDrawElements(GL_TRIANGLES,section.indexCount,GL_UNSIGNED_INT,(void*)(section.indexOffset * sizeof(uint32))
                );
            }

            ImmediatePipeline::Get().End();

            glBindVertexArray(0);

        }

        Vector<Vertex>& GetVertices() { return m_vertices; }
        Vector<uint32>& GetIndices() { return m_indices; }
        Vector<MeshSection>& GetSections() { return m_sections; }
        Vector<ModelMaterial>& GetMaterials() { return m_materials; }
        SharedPtr<Mesh> GetMesh() { return m_mesh; }

        void SetName(const String& inName) { m_name = inName; }
        const String& GetName() const { return m_name; }

        void SetMaterial(SharedPtr<Material> inShader) { m_material = inShader; }
        SharedPtr<Material> GetMaterial() const { return m_material; }

        void SetMesh(SharedPtr<Mesh> inMesh) { m_mesh = inMesh; }
        SharedPtr<Mesh> GetMesh() const { return m_mesh; }

    private:
        String m_name;
        Vector<Vertex> m_vertices;
        Vector<uint32> m_indices;

        Vector<MeshSection> m_sections;
        Vector<ModelMaterial> m_materials;

        SharedPtr<Mesh> m_mesh = nullptr;
        SharedPtr<Material> m_material = nullptr;
    };

    // Stores the attributes of a circle
    struct SphereDesc {
    public:
        int32 res = 100;
        Texture* texture = nullptr;
        Math::Vec3f baseColor = Math::Vec3f(1.00, 1.00, 1.00);
        Math::Vec3f emissiveColor = Math::Vec3f(0.00, 0.00, 0.00);
        float32 emissiveStrength = 1.0;
    };

    class Sphere {
    public:
        Mesh m_sphereMesh;
        Material m_material;

        Sphere() : Sphere(SphereDesc()) {}
        Sphere(const SphereDesc& circleDesc)
        {
            m_sphereMesh = CreateSphereMesh(circleDesc.res);
            
            Shader shader = ResourceManager::GetShader(
                "SphereShader",
                R"(Nyx\Source\Application\Shaders\Sphere\sphere.vert)",
                R"(Nyx\Source\Application\Shaders\Sphere\sphere.frag)"
            );

            m_material = Material(shader, circleDesc.baseColor, circleDesc.emissiveColor, circleDesc.emissiveStrength, circleDesc.texture);
        }

        ~Sphere()
        {
            // Will get moved to a SceneManager, where it will handle the lifetime of a Sphere.
            // glDeleteVertexArrays(1, &m_sphereMesh.circleVAO);
        }

        Mesh CreateSphereMesh(const uint32 res)
        {
            Vector<float32> vertices = GenerateVertices(res);
            Vector<uint32> indices = GenerateIndices(res);

            Mesh mesh;

            glGenVertexArrays(1, &mesh.vao.m_data);
            glGenBuffers(1, &mesh.vbo.m_data);
            glGenBuffers(1, &mesh.ebo.m_data);

            glBindVertexArray(mesh.vao.m_data);

            glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo.m_data);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo.m_data);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            // Position attribute (location = 0)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // UV attribute (location = 1)
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // Normal attribute (location = 2)
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);

            mesh.ebo.m_indexCount = static_cast<uint32>(indices.size());

            return mesh;
        }

        void DrawSphere(Math::Mat4f& model, Math::Mat4f& view, Math::Mat4f& projection)
        {
            m_material.Bind();

            uint32 shaderID = m_material.GetShader().GetID();
            LightingSystem::Get().UploadToShader(shaderID);

            GLuint modelLoc = glGetUniformLocation(shaderID, "uModel");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            GLuint viewLoc = glGetUniformLocation(shaderID, "uView");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            GLuint projLoc = glGetUniformLocation(shaderID, "uProj");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            glBindVertexArray(m_sphereMesh.vao.m_data);

            ImmediatePipeline::Get().Begin();
            ImmediatePipeline::Get().UseSphere();
            glDrawElements(GL_TRIANGLES, m_sphereMesh.ebo.m_indexCount, GL_UNSIGNED_INT, 0);
            ImmediatePipeline::Get().End();

            glBindVertexArray(0);

        }

    private:
        Math::Vec3f GetCubeFacePosition(uint32 face, float u, float v)
        {
            // Map u,v from [0,1] to [-1,1]
            float x = 2.0f * u - 1.0f;
            float y = 2.0f * v - 1.0f;

            switch (face)
            {
            case 0: return Math::Vec3f(1, y, -x);  // +X face
            case 1: return Math::Vec3f(-1, y, x);  // -X face
            case 2: return Math::Vec3f(x, 1, -y);  // +Y face
            case 3: return Math::Vec3f(x, -1, y);  // -Y face
            case 4: return Math::Vec3f(x, y, 1);   // +Z face
            case 5: return Math::Vec3f(-x, y, -1); // -Z face
            }
            return Math::Vec3f(0, 0, 0);
        }

        Vector<float32> GenerateVertices(const uint32 resolution)
        {
            Vector<float32> vertices;

            for (int y = 0; y <= resolution; ++y)
            {
                float v = (float)y / resolution; // V coordinate

                for (int x = 0; x <= resolution; ++x)
                {
                    float u = 1.0f - ((float)x / resolution); // flipped U

                    float theta = (1.0f - u) * glm::two_pi<float>(); // longitude [0, 2PI]
                    float phi = v * glm::pi<float>();       // latitude  [0, PI]

                    Math::Vec3f pos;
                    pos.x = sinf(phi) * cosf(theta);
                    pos.y = cosf(phi);
                    pos.z = sinf(phi) * sinf(theta);

                    Math::Vec3f norm = glm::normalize(pos);

                    vertices.push_back(pos.x);
                    vertices.push_back(pos.y);
                    vertices.push_back(pos.z);

                    vertices.push_back(u); // flipped U
                    vertices.push_back(v); // V

                    vertices.push_back(norm.x);
                    vertices.push_back(norm.y);
                    vertices.push_back(norm.z);
                }
            }

            return vertices;
        }

        Vector<uint32> GenerateIndices(const uint32 resolution)
        {
            Vector<uint32> indices;

            for (int y = 0; y < resolution; ++y)
            {
                for (int x = 0; x < resolution; ++x)
                {
                    int i0 = y * (resolution + 1) + x;
                    int i1 = i0 + 1;
                    int i2 = i0 + (resolution + 1);
                    int i3 = i2 + 1;

                    // Two triangles per quad
                    indices.push_back(i0);
                    indices.push_back(i2);
                    indices.push_back(i1);

                    indices.push_back(i1);
                    indices.push_back(i2);
                    indices.push_back(i3);
                }
            }

            return indices;
        }
    };
}



