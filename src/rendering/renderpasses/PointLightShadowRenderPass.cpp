/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "PointLightShadowRenderPass.h"

#include "core/FileSystem.h"
#include "core/Vertex.h"
#include "data/PointLight.h"
#include "data/Mesh.h"
#include "data/Texture.h"
#include "rendering/Framebuffer.h"
#include "rendering/LightTransform.h"
#include "rendering/MeshBuffer.h"
#include "rendering/SceneData.h"
#include "rendering/Shader.h"
#include "rendering/VertexLayout.h"

#include <glm/glm.hpp>

struct alignas(16) LightTransformUbo
{
        glm::mat4 lightSpaceMatrix;
        glm::mat4 model;
};

struct alignas(16) LightPositionUbo
{
        glm::vec3 lightPosition;
        float _padding;
};

struct alignas(16) FarPlaneUbo
{
        float farPlane;
        float _padding[3];
};

constexpr auto maxPointLights = 8;
constexpr auto shadowMapWidth = 2048;
constexpr auto shadowMapHeight = 2048;

PointLightShadowRenderPass::PointLightShadowRenderPass()
 : RenderPass()
{
    const auto shaderDir = GetShaderDir();
    const auto vsPath = shaderDir / "mesh_pointlight_shadow_vertex.glsl";
    const auto fsPath = shaderDir / "mesh_pointlight_shadow_fragment.glsl";

    m_shader = std::make_unique<Shader>(vsPath, fsPath);
    m_shader->registerUniformBuffer("LightTransformBlock", sizeof(LightTransformUbo), 0);
    m_shader->registerUniformBuffer("LightPositionBlock", sizeof(LightPositionUbo), 1);
    m_shader->registerUniformBuffer("FarPlaneBlock", sizeof(FarPlaneUbo), 2);

    m_framebuffer = std::make_unique<Framebuffer>();
    m_framebuffer->setDrawBuffer(GL_NONE);
    m_framebuffer->setReadBuffer(GL_NONE);

    m_vertexLayout = std::make_unique<VertexLayout>();
    m_vertexLayout->registerAttribute(0, 3, GL_FLOAT, offsetof(Vertex, position));
    m_vertexLayout->registerAttribute(1, 2, GL_FLOAT, offsetof(Vertex, textureUV));
    m_vertexLayout->registerAttribute(2, 3, GL_FLOAT, offsetof(Vertex, normal));

    m_pointLightDepthImage = std::make_unique<TextureCubeMapArray>(GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight, 6 * maxPointLights);
    m_pointLightDepthImage->setMinFilter(GL_LINEAR);
    m_pointLightDepthImage->setMagFilter(GL_LINEAR);
    m_pointLightDepthImage->setWrapS(GL_CLAMP_TO_EDGE);
    m_pointLightDepthImage->setWrapT(GL_CLAMP_TO_EDGE);
    m_pointLightDepthImage->setWrapR(GL_CLAMP_TO_EDGE);
}

PointLightShadowRenderPass::~PointLightShadowRenderPass() = default;


void PointLightShadowRenderPass::execute(const std::vector<DrawCommand>& drawQueue, 
                               const SceneData& sceneData, 
                               const MeshBuffer& buffer)
{
    m_shader->bind();
    m_framebuffer->bind();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    const float farPlane = 50.0f;

    auto lightIndex = 0;
    for (const auto& light : sceneData.pointLights)
    {
        const auto lightTransforms = getPointLightShadowTransforms(light.position, farPlane);

        for (auto i = 0; i < 6; ++i)
        {
            m_framebuffer->attachTextureLayer(GL_DEPTH_ATTACHMENT, *m_pointLightDepthImage.get(), 0, (6 * lightIndex) + i);
            glViewport(0, 0, shadowMapWidth, shadowMapHeight);
            glClear(GL_DEPTH_BUFFER_BIT);


            const auto lightPosUbo = LightPositionUbo{.lightPosition = light.position};
            m_shader->writeUniformData("LightPositionBlock", sizeof(LightPositionUbo), &lightPosUbo);

            const auto farPlaneUbo = FarPlaneUbo{.farPlane = farPlane};
            m_shader->writeUniformData("FarPlaneBlock", sizeof(FarPlaneUbo), &farPlaneUbo);

            for (const auto& drawCommand : drawQueue)
            {
                const auto indexCount = drawCommand.instance.mesh->indices().size();
                const auto indexOffset = buffer.indexOffsetOfMesh(drawCommand.instance.mesh);
                const auto vertexOffset = buffer.vertexOffsetOfMesh(drawCommand.instance.mesh);

                auto pointLightTransformUbo = LightTransformUbo{};
                pointLightTransformUbo.lightSpaceMatrix = lightTransforms.at(i);
                pointLightTransformUbo.model = drawCommand.transform;
                m_shader->writeUniformData("LightTransformBlock", sizeof(LightTransformUbo), &pointLightTransformUbo);

                glDrawElementsBaseVertex(
                    GL_TRIANGLES,
                    static_cast<GLsizei>(indexCount),
                    GL_UNSIGNED_INT,
                    reinterpret_cast<const void*>(indexOffset * sizeof(GLuint)),
                    vertexOffset);
            }
            
        }
        ++lightIndex;
    }
}

TextureCubeMapArray* PointLightShadowRenderPass::pointLightShadowMapImage() const
{
    return m_pointLightDepthImage.get();
}
