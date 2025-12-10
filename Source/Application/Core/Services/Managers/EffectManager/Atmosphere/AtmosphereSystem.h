#pragma once

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx {

    class AtmosphereSystem : public Singleton<AtmosphereSystem> {
    public:
        void DrawAtmosphere(Camera& camera) 
        {
            for (auto entityID : ECS::Get().View<Atmosphere, Transform>()) {
                const auto& atmosphere = ECS::Get().GetComponent<Atmosphere>(entityID);
                const auto& atmosphereDesc = atmosphere->m_atmosphereDesc;
                auto& mat = atmosphere->m_atmosphereMaterial;
                auto* transform = ECS::Get().GetComponent<Transform>(entityID);

                // Scale it down for rendering purposes
                auto pos = transform->position / METERS_PER_UNIT;
                auto sca = transform->scale / METERS_PER_UNIT;

                const float radius = sca.getRadius();
                const float outerRadius = radius + atmosphereDesc.atmosphereThickness;

                const auto& mesh = atmosphereDesc.atmosphereMesh;
                if (!mesh) 
                    continue;

                mat.Bind();
                uint32 shaderID = mat.GetShader().GetID();

                // Standard camera uniforms
                GL::Get()->glUniformMatrix4fv(GL::Get()->glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
                GL::Get()->glUniformMatrix4fv(GL::Get()->glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));

                // Per-instance uniforms
                Position cameraPos = camera.GetPosition();
                glm::vec3 center = pos.GetWorld() - cameraPos.GetWorld();
                glm::mat4 model = glm::translate(glm::mat4(1.0f), center);
                model = glm::scale(model, glm::vec3(outerRadius));

                GL::Get()->glUniform3fv(GL::Get()->glGetUniformLocation(shaderID, "uCameraPos"), 1, glm::value_ptr(glm::vec3(0.0f)));
                GL::Get()->glUniform3fv(GL::Get()->glGetUniformLocation(shaderID, "uPlanetCenter"), 1, glm::value_ptr(center));
                GL::Get()->glUniformMatrix4fv(GL::Get()->glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
                GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, "uPlanetRadius"), radius);
                GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, "uAtmosphereRadius"), outerRadius);
                GL::Get()->glUniform3fv(GL::Get()->glGetUniformLocation(shaderID, "uScatteringColor"), 1, glm::value_ptr(atmosphereDesc.scatteringColor));
                GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, "uDensityFalloff"), atmosphereDesc.densityFalloff);
                GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, "uEdgeStrength"), atmosphereDesc.edgeStrength);
                GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, "uCenterStrength"), atmosphereDesc.centerStrength);
                GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, "uEdgePower"), atmosphereDesc.edgePower);
                GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, "uCenterPower"), atmosphereDesc.centerPower);
                GL::Get()->glUniform1f(GL::Get()->glGetUniformLocation(shaderID, "uExposure"), atmosphereDesc.exposure);

                // Upload lights
                LightingSystem::Get().UploadToShader(shaderID);

                GL::Get()->glBindVertexArray(mesh->vao.m_data);

                ImmediatePipeline::Get().Begin();
                ImmediatePipeline::Get().UseAtmosphere();
                glDrawElements(GL_TRIANGLES, mesh->ebo.m_indexCount, GL_UNSIGNED_INT, 0);
                ImmediatePipeline::Get().End();
            }
        }
    };

}