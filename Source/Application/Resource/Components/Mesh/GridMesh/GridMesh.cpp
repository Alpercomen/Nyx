#pragma once
#include "GridMesh.h"

GridMesh::GridMesh() 
{
    m_shader = ResourceManager::GetShader(
        "GridShader",
        R"(Nyx\Source\Application\Shaders\Grid\grid.vert)",
        R"(Nyx\Source\Application\Shaders\Grid\grid.frag)"
    );

}

void GridMesh::DrawGrid(Camera& camera)
{
    m_shader.Use();

    Math::Mat4f view = camera.GetViewMatrix();
    Math::Mat4f projection = camera.GetProjectionMatrix();
    Math::Vec3f cameraPos = camera.GetPosition().GetWorld();

    GLuint uView = GL::Get()->glGetUniformLocation(m_shader.GetID(), "uView");
    GL::Get()->glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));

    GLuint uProj = GL::Get()->glGetUniformLocation(m_shader.GetID(), "uProj");
    GL::Get()->glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projection));

    GLuint uNear = GL::Get()->glGetUniformLocation(m_shader.GetID(), "uNear");
    GL::Get()->glUniform1f(uNear, camera.GetNearPlane());

    GLuint uFar = GL::Get()->glGetUniformLocation(m_shader.GetID(), "uFar");
    GL::Get()->glUniform1f(uFar, camera.GetFarPlane());

    GLuint uCameraPos = GL::Get()->glGetUniformLocation(m_shader.GetID(), "uCameraPos");
    GL::Get()->glUniform3fv(uCameraPos, 1, glm::value_ptr(cameraPos));

    ImmediatePipeline::Get().Begin();
    ImmediatePipeline::Get().UseGrid();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ImmediatePipeline::Get().End();

}