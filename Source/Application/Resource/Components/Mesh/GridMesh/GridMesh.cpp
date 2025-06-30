#include "GridMesh.h"

GridMesh::GridMesh() 
{
    m_shader = Shader(
        R"(D:\Documents\Projects\Nyx\Source\Application\Shaders\Grid\grid.vert)",
        R"(D:\Documents\Projects\Nyx\Source\Application\Shaders\Grid\grid.frag)"
    );

}

void GridMesh::DrawGrid(const Camera& camera) const
{
    m_shader.Use();

    Math::Mat4f view = camera.GetViewMatrix();
    Math::Mat4f projection = camera.GetProjectionMatrix();

    GLuint uView = glGetUniformLocation(m_shader.GetID(), "uView");
    glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));

    GLuint uProj = glGetUniformLocation(m_shader.GetID(), "uProj");
    glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projection));

    GLuint uNear = glGetUniformLocation(m_shader.GetID(), "uNear");
    glUniform1f(uNear, camera.GetNearPlane());

    GLuint uFar = glGetUniformLocation(m_shader.GetID(), "uFar");
    glUniform1f(uFar, camera.GetFarPlane());

    ImmediatePipeline::Get().Begin();
    ImmediatePipeline::Get().UseGrid();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ImmediatePipeline::Get().End();

}