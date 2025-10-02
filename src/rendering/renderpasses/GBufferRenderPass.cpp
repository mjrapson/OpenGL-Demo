/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "GBufferRenderPass.h"

#include "core/FileSystem.h"
#include "core/Vertex.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/Texture.h"
#include "rendering/Camera.h"
#include "rendering/Framebuffer.h"
#include "rendering/MeshBuffer.h"
#include "rendering/SceneData.h"
#include "rendering/Shader.h"
#include "rendering/VertexLayout.h"

#include <glm/glm.hpp>

struct alignas(16) TransformUbo
{
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
};

struct alignas(16) MaterialUbo
{
        int hasTexture{0};
        int _padding[3];
        glm::vec4 diffuseColor{0.0f, 0.0f, 0.0f, 1.0f};
};

GBufferRenderPass::GBufferRenderPass()
    : RenderPass()
{
    const auto shaderDir = GetShaderDir();
    const auto vsPath = shaderDir / "mesh_deferred_vertex.glsl";
    const auto fsPath = shaderDir / "mesh_deferred_fragment.glsl";

    m_shader = std::make_unique<Shader>(vsPath, fsPath);
    m_shader->registerUniformBuffer("TransformBlock", sizeof(TransformUbo), 0);
    m_shader->registerTextureSampler("diffuseTexture", 1);
    m_shader->registerUniformBuffer("MaterialBlock", sizeof(MaterialUbo), 2);

    m_framebuffer = std::make_unique<Framebuffer>();
    m_framebuffer->setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});

    m_vertexLayout = std::make_unique<VertexLayout>();
    m_vertexLayout->registerAttribute(0, 3, GL_FLOAT, offsetof(Vertex, position));
    m_vertexLayout->registerAttribute(1, 2, GL_FLOAT, offsetof(Vertex, textureUV));
    m_vertexLayout->registerAttribute(2, 3, GL_FLOAT, offsetof(Vertex, normal));
}

GBufferRenderPass::~GBufferRenderPass() = default;

void GBufferRenderPass::execute(const std::vector<DrawCommand>& drawQueue, 
                                const SceneData& sceneData, 
                                const MeshBuffer& buffer)
{
    m_shader->bind();
    m_framebuffer->bind();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
    m_vertexLayout->bind();
    buffer.bindToVertexLayout(*m_vertexLayout);

    for (const auto& drawCommand : drawQueue)
    {
        const auto indexCount = drawCommand.instance.mesh->indices().size();
        const auto indexOffset = buffer.indexOffsetOfMesh(drawCommand.instance.mesh);
        const auto vertexOffset = buffer.vertexOffsetOfMesh(drawCommand.instance.mesh);

        const auto* material = drawCommand.instance.material;

        auto transformUbo = TransformUbo{};
        transformUbo.projection = sceneData.camera->projection();
        transformUbo.view = sceneData.camera->view();
        transformUbo.model = drawCommand.transform;

        auto materialUbo = MaterialUbo{};
        materialUbo.diffuseColor = glm::vec4{material->diffuse, 1.0f};
        if (material->diffuseTexture)
        {
            materialUbo.hasTexture = 1;
            m_shader->bindTexture("diffuseTexture", material->diffuseTexture.value());
        }
        else
        {
            materialUbo.hasTexture = 0;
        }
        m_shader->writeUniformData("TransformBlock", sizeof(TransformUbo), &transformUbo);
        m_shader->writeUniformData("MaterialBlock", sizeof(MaterialUbo), &materialUbo);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(indexCount),
            GL_UNSIGNED_INT,
            reinterpret_cast<const void*>(indexOffset * sizeof(GLuint)),
            vertexOffset);
    }
}

void GBufferRenderPass::onViewportResize(GLuint width, GLuint height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;

    rebuildImages();
}

Texture2D* GBufferRenderPass::colorImage() const
{
    return m_colorImage.get();
}

Texture2D* GBufferRenderPass::normalImage() const
{
    return m_normalImage.get();
}

Texture2D* GBufferRenderPass::positionImage() const
{
    return m_positionImage.get();
}

Texture2D* GBufferRenderPass::depthImage() const
{
    return m_depthImage.get();
}

void GBufferRenderPass::rebuildImages()
{
    m_colorImage = std::make_unique<Texture2D>(GL_RGBA8, m_viewportWidth, m_viewportHeight);
    m_colorImage->setMinFilter(GL_LINEAR);
    m_colorImage->setMagFilter(GL_LINEAR);

    m_normalImage = std::make_unique<Texture2D>(GL_RGBA16F, m_viewportWidth, m_viewportHeight);
    m_normalImage->setMinFilter(GL_LINEAR);
    m_normalImage->setMagFilter(GL_LINEAR);

    m_positionImage = std::make_unique<Texture2D>(GL_RGBA32F, m_viewportWidth, m_viewportHeight);
    m_positionImage->setMinFilter(GL_NEAREST);
    m_positionImage->setMagFilter(GL_NEAREST);

    m_depthImage = std::make_unique<Texture2D>(GL_DEPTH_COMPONENT24, m_viewportWidth, m_viewportHeight);

    m_framebuffer->attachTexture(GL_COLOR_ATTACHMENT0, *m_colorImage.get(), 0);
    m_framebuffer->attachTexture(GL_COLOR_ATTACHMENT1, *m_normalImage.get(), 0);
    m_framebuffer->attachTexture(GL_COLOR_ATTACHMENT2, *m_positionImage.get(), 0);
    m_framebuffer->attachTexture(GL_DEPTH_ATTACHMENT, *m_depthImage.get(), 0);
}
