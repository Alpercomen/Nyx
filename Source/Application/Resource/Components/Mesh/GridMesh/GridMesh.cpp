#include "GridMesh.h"

GridMesh::GridMesh() 
{
    m_shader = ResourceManager::GetShader(
        "GridShader",
        R"(Nyx\Source\Application\Shaders\Grid\grid.vert)",
        R"(Nyx\Source\Application\Shaders\Grid\grid.frag)"
    );

}

void GridMesh::DrawGrid(const Camera& camera, const Transform& cameraTransform) const
{
    m_shader.Use();

    Math::Mat4f view = camera.GetViewMatrix();
    Math::Mat4f projection = camera.GetProjectionMatrix();
    Math::Vec3f cameraPos = cameraTransform.position.GetWorld();

    GLuint uView = glGetUniformLocation(m_shader.GetID(), "uView");
    glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));

    GLuint uProj = glGetUniformLocation(m_shader.GetID(), "uProj");
    glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projection));

    GLuint uNear = glGetUniformLocation(m_shader.GetID(), "uNear");
    glUniform1f(uNear, camera.GetNearPlane());

    GLuint uFar = glGetUniformLocation(m_shader.GetID(), "uFar");
    glUniform1f(uFar, camera.GetFarPlane());

    GLuint uCameraPos = glGetUniformLocation(m_shader.GetID(), "uCameraPos");
    glUniform3fv(uCameraPos, 1, glm::value_ptr(cameraPos));

    ImmediatePipeline::Get().Begin();
    ImmediatePipeline::Get().UseGrid();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ImmediatePipeline::Get().End();

}