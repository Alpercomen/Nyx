#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Core/Services/Managers/ResourceManager/ResourceManager.h>
#include <Application/Core/Services/Pipeline/Immediate/Immediate.h>
#include <Application/Core/Services/ResourceLocator/ResourceLocator.h>

#include <Application/Resource/Components/Transform/Position.h>
#include <Application/Resource/Components/Rigidbody/Velocity.h>
#include <Application/Resource/Components/Rigidbody/Acceleration.h>
#include <Application/Core/Physics/Meter.h>
#include <Application/Resource/Camera/Camera.h>
#include <Application/Resource/Material/ShaderProgram/ShaderProgram.h>
#include <Application/Resource/Buffers/VAO.h>
#include <Application/Resource/Buffers/VBO.h>
#include <Application/Resource/Buffers/EBO.h>

namespace Nyx
{
    struct Mesh
    {
        VAO vao;
        VBO vbo;
        EBO ebo;
    };

    // Stores the attributes of a circle
    struct SphereDesc {
    public:
        int32 res = 50;

        Math::Vec3f topColor;
        Math::Vec3f botColor;
    };

    class Sphere {
    public:
        SphereDesc m_sphereDesc;
        Mesh m_sphereMesh;
        Shader m_shader;

        Sphere() : Sphere(SphereDesc()) {}
        Sphere(const SphereDesc& circleDesc) : m_sphereDesc(circleDesc)
        {
            m_sphereMesh = CreateSphereMesh(m_sphereDesc);
            m_shader = ResourceManager::GetShader(
                "SphereShader",
                R"(Nyx\Source\Application\Shaders\Sphere\sphere.vert)",
                R"(Nyx\Source\Application\Shaders\Sphere\sphere.frag)"
            );
        }

        ~Sphere()
        {
            // Will get moved to a SceneManager, where it will handle the lifetime of a Sphere.
            // glDeleteVertexArrays(1, &m_sphereMesh.circleVAO);
        }

        Mesh CreateSphereMesh(const SphereDesc& circleDesc)
        {
            const uint32 X_SEGMENTS = circleDesc.res;
            const uint32 Y_SEGMENTS = circleDesc.res;

            Vector<float32> vertices = GenerateVertices(X_SEGMENTS, Y_SEGMENTS);
            Vector<uint32> indices = GenerateIndices(X_SEGMENTS, Y_SEGMENTS); 

            Mesh mesh;

            glGenVertexArrays(1, &mesh.vao.m_data);
            glGenBuffers(1, &mesh.vbo.m_data);
            glGenBuffers(1, &mesh.ebo.m_data);

            glBindVertexArray(mesh.vao.m_data);

            glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo.m_data);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo.m_data);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);

            mesh.ebo.m_indexCount = static_cast<uint32>(indices.size());

            return mesh;
        }

        void DrawSphere(Math::Mat4f mvp)
        {
            m_shader.Use(); // Use Shader

            GLuint mvpLoc = glGetUniformLocation(m_shader.GetID(), "uMVP");
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

            GLuint topColorLoc = glGetUniformLocation(m_shader.GetID(), "topColor");
            glUniform3fv(topColorLoc, 1, glm::value_ptr(m_sphereDesc.topColor));

            GLuint botColorLoc = glGetUniformLocation(m_shader.GetID(), "botColor");
            glUniform3fv(botColorLoc, 1, glm::value_ptr(m_sphereDesc.botColor));

            glBindVertexArray(m_sphereMesh.vao.m_data);

            ImmediatePipeline::Get().Begin();
            ImmediatePipeline::Get().UseSphere();
            glDrawElements(GL_TRIANGLE_STRIP, m_sphereMesh.ebo.m_indexCount, GL_UNSIGNED_INT, 0);
            ImmediatePipeline::Get().End();

        }

    private:
        Vector<float32> GenerateVertices(const uint32 X_SEGMENTS, const uint32 Y_SEGMENTS)
        {
            Vector<float32> vertices;

            for (uint32 y = 0; y <= Y_SEGMENTS; ++y)
            {
                for (uint32 x = 0; x <= X_SEGMENTS; ++x)
                {
                    float xSegment = (float)x / (float)X_SEGMENTS;
                    float ySegment = (float)y / (float)Y_SEGMENTS;
                    float xPos = cos(xSegment * 2.0f * Math::Pi) * sin(ySegment * Math::Pi);
                    float yPos = cos(ySegment * Math::Pi);
                    float zPos = sin(xSegment * 2.0f * Math::Pi) * sin(ySegment * Math::Pi);

                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);
                }
            }

            return vertices;
        }

        Vector<uint32> GenerateIndices(const uint32 X_SEGMENTS, const uint32 Y_SEGMENTS)
        {
            Vector<uint32> indices;

            bool oddRow = false;
            for (uint32 y = 0; y < Y_SEGMENTS; ++y)
            {
                if (!oddRow)
                {
                    for (uint32 x = 0; x <= X_SEGMENTS; ++x)
                    {
                        indices.push_back(y * (X_SEGMENTS + 1) + x);
                        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    }
                }
                else
                {
                    for (int32 x = X_SEGMENTS; x >= 0; --x)
                    {
                        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                        indices.push_back(y * (X_SEGMENTS + 1) + x);
                    }
                }
                oddRow = !oddRow;
            }

            return indices;
        }
    };
}



