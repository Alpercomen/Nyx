#include "Skybox.h"

#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace Nyx
{
    Skybox::Skybox(const Vector<String>& faces)
    {
        try
        {
            m_shader = ResourceManager::GetShader(
                "SkyboxShader",
                R"(Nyx\Source\Application\Shaders\Skybox\skybox.vert)",
                R"(Nyx\Source\Application\Shaders\Skybox\skybox.frag)"
            );
        }
        catch (const std::exception& e)
        {
            spdlog::error("Skybox initialization exception: {}", e.what());
        }

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindVertexArray(0);

        glGenTextures(1, &m_cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (size_t i = 0; i < faces.size(); ++i)
        {
            String facePath = ResourceLocator::Get(faces[i]);
            TextureData img = TextureLoader::Load(facePath, false);

            if (!img.IsValid())
            {
                spdlog::error("Skybox failed to load face: {}", facePath);
                continue;
            }

            GLenum format = GL_RGB;
            if (img.channels == 1)
                format = GL_RED;
            else if (img.channels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, format, img.width, img.height, 0, format, GL_UNSIGNED_BYTE, img.pixels);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            TextureLoader::Free(img);
        }


        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        spdlog::info("Skybox initialized: cubemap ID = {} | faces loaded = {}", m_cubemap, (int)faces.size());
    }

    Skybox::~Skybox()
    {
        if (m_vao)
            glDeleteVertexArrays(1, &m_vao);

        if (m_cubemap)
            glDeleteTextures(1, &m_cubemap);
    }

    Skybox::Skybox(Skybox&& other) noexcept
        : m_vao(other.m_vao), m_cubemap(other.m_cubemap), m_shader(std::move(other.m_shader))
    {
        other.m_vao = 0;
        other.m_cubemap = 0;
    }

    Skybox& Skybox::operator=(Skybox&& other) noexcept
    {
        if (this != &other)
        {
            if (m_vao)
                glDeleteVertexArrays(1, &m_vao);
            if (m_cubemap)
                glDeleteTextures(1, &m_cubemap);

            m_vao = other.m_vao;
            m_cubemap = other.m_cubemap;
            m_shader = std::move(other.m_shader);

            other.m_vao = 0;
            other.m_cubemap = 0;
        }

        return *this;
    }

    void Skybox::Draw(const Math::Mat4f& view, const Math::Mat4f& proj) const
    {
        m_shader.Use();
        uint32 shaderID = m_shader.GetID();

        Math::Mat4f viewNoTrans = glm::mat4(glm::mat3(view));

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(viewNoTrans));
        GLint uViewLoc = glGetUniformLocation(shaderID, "uView");
        GLint uProjLoc = glGetUniformLocation(shaderID, "uProj");
        glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, glm::value_ptr(viewNoTrans));
        glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(m_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
        GLint skyboxLoc = glGetUniformLocation(shaderID, "skybox");
        glUniform1i(skyboxLoc, 0);

        spdlog::debug("Skybox.Draw uniforms: uView={}, uProj={}, skybox={}", uViewLoc, uProjLoc, skyboxLoc);

        ImmediatePipeline::Get().Begin();
        ImmediatePipeline::Get().UseSkybox();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        ImmediatePipeline::Get().End();

        glBindVertexArray(0);
    }
}