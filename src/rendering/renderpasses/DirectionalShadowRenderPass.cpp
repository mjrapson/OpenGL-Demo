/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "DirectionalShadowRenderPass.h"

#include "core/FileSystem.h"
#include "core/Vertex.h"
#include "data/DirectionalLight.h"
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

constexpr auto ShadowMapWidth = 2048;
constexpr auto ShadowMapHeight = 2048;

DirectionalShadowRenderPass::DirectionalShadowRenderPass()
    : RenderPass()
{
    const auto shaderDir = GetShaderDir();
    const auto vsPath = shaderDir / "mesh_shadow_vertex.glsl";
    const auto fsPath = shaderDir / "mesh_shadow_fragment.glsl";

    m_shader = std::make_unique<Shader>(vsPath, fsPath);
    m_shader->registerUniformBuffer("LightTransformBlock", sizeof(LightTransformUbo), 0);

    m_framebuffer = std::make_unique<Framebuffer>();
    m_framebuffer->setDrawBuffer(GL_NONE);
    m_framebuffer->setReadBuffer(GL_NONE);

    m_vertexLayout = std::make_unique<VertexLayout>();
    m_vertexLayout->registerAttribute(0, 3, GL_FLOAT, offsetof(Vertex, position));
    m_vertexLayout->registerAttribute(1, 2, GL_FLOAT, offsetof(Vertex, textureUV));
    m_vertexLayout->registerAttribute(2, 3, GL_FLOAT, offsetof(Vertex, normal));

    m_shadowMapDepthImage = std::make_unique<Texture2D>(GL_DEPTH_COMPONENT24, ShadowMapWidth, ShadowMapHeight);
    m_shadowMapDepthImage->setMinFilter(GL_LINEAR);
    m_shadowMapDepthImage->setMagFilter(GL_LINEAR);
    m_shadowMapDepthImage->setWrapS(GL_CLAMP_TO_BORDER);
    m_shadowMapDepthImage->setWrapT(GL_CLAMP_TO_BORDER);
    m_shadowMapDepthImage->setBorderColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    m_shadowMapDepthImage->setComparisonMode(GL_COMPARE_REF_TO_TEXTURE);
    m_shadowMapDepthImage->setComparisonFunction(GL_LEQUAL);
}

DirectionalShadowRenderPass::~DirectionalShadowRenderPass() = default;

void DirectionalShadowRenderPass::execute(const std::vector<DrawCommand>& drawQueue, 
                                          const SceneData& sceneData, 
                                          const MeshBuffer& buffer)
{
    m_shader->bind();
    m_framebuffer->bind();
    m_framebuffer->attachTexture(GL_DEPTH_ATTACHMENT, *m_shadowMapDepthImage.get(), 0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, ShadowMapWidth, ShadowMapHeight);
    m_vertexLayout->bind();
    buffer.bindToVertexLayout(*m_vertexLayout);

    auto lightTransformUbo = LightTransformUbo{};
    lightTransformUbo.lightSpaceMatrix = getLightSpaceMatrix(sceneData.directionalLight.direction);

    for (const auto& drawCommand : drawQueue)
    {
        const auto indexCount = drawCommand.instance.mesh->indices().size();
        const auto indexOffset = buffer.indexOffsetOfMesh(drawCommand.instance.mesh);
        const auto vertexOffset = buffer.vertexOffsetOfMesh(drawCommand.instance.mesh);

        lightTransformUbo.model = drawCommand.transform;

        m_shader->writeUniformData("LightTransformBlock", sizeof(LightTransformUbo), &lightTransformUbo);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(indexCount),
            GL_UNSIGNED_INT,
            reinterpret_cast<const void*>(indexOffset * sizeof(GLuint)),
            vertexOffset);
    }
}

Texture2D* DirectionalShadowRenderPass::directionalLightShadowMapImage() const
{
    return m_shadowMapDepthImage.get();
}
