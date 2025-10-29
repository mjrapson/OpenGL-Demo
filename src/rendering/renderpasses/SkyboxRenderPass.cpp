/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "SkyboxRenderPass.h"

#include "core/FileSystem.h"
#include "data/Mesh.h"
#include "data/Skybox.h"
#include "data/Texture.h"
#include "rendering/Camera.h"
#include "rendering/Framebuffer.h"
#include "rendering/MeshBuffer.h"
#include "rendering/Shader.h"
#include "rendering/VertexLayout.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

const auto quadVertices =
    std::vector<float>{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
                       1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f};

struct alignas(16) TransformUbo
{
        glm::mat4 projection;
        glm::mat4 view;
};

SkyboxRenderPass::SkyboxRenderPass()
    : RenderPass()
{
    const auto shaderDir = GetShaderDir();
    const auto vsPath = shaderDir / "skybox_vertex.glsl";
    const auto fsPath = shaderDir / "skybox_fragment.glsl";

    m_shader = std::make_unique<Shader>(vsPath, fsPath);
    m_shader->registerUniformBuffer("TransformBlock", sizeof(TransformUbo), 0);
    m_shader->registerTextureSampler("skyboxTexture", 1);

    m_framebuffer = std::make_unique<Framebuffer>();
    m_framebuffer->setDrawBuffers({GL_COLOR_ATTACHMENT0});

    m_vertexLayout = std::make_unique<VertexLayout>();
    m_vertexLayout->registerAttribute(0, 2, GL_FLOAT, 0);
    m_vertexLayout->registerAttribute(1, 2, GL_FLOAT, 2 * sizeof(float));

    m_vertexBuffer = std::make_unique<Buffer<float>>(quadVertices);
    m_vertexLayout->bindVertexBuffer(0, m_vertexBuffer->handle(), 0, 4 * sizeof(float));
}

SkyboxRenderPass::~SkyboxRenderPass() = default;


void SkyboxRenderPass::execute(const std::vector<DrawCommand>& drawQueue, 
                               const Camera& camera,
                               const DirectionalLight& directionalLight,
                               const std::vector<PointLight>& pointLights,
                               const MeshBuffer& buffer)
{
    if(!camera.skybox)
    {
        return;
    }

    m_shader->bind();
    m_framebuffer->bind();

    const auto inputs = m_inputBinding();
    m_framebuffer->attachTexture(GL_COLOR_ATTACHMENT0, *inputs.targetImage, 0);

    m_shader->bindTexture("skyboxTexture", camera.skybox.value()->cubemapTexture.get());

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA, GL_ONE, GL_ZERO);

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
    m_vertexLayout->bind();

    auto transformUbo = TransformUbo{};
    transformUbo.projection = glm::perspective(camera.fieldOfView, m_aspectRatio, camera.nearPlane, camera.farPlane);
    transformUbo.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

    m_shader->writeUniformData("TransformBlock", sizeof(TransformUbo), &transformUbo);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SkyboxRenderPass::onViewportResize(GLuint width, GLuint height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;

    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    rebuildImages();
}

void SkyboxRenderPass::setInputBinding(InputBinding binding)
{
    m_inputBinding = std::move(binding);
}

Texture2D* SkyboxRenderPass::colorImage() const
{
    return m_colorImage.get();
}

GLuint SkyboxRenderPass::framebufferHandle() const
{
    return m_framebuffer->handle();
}

void SkyboxRenderPass::rebuildImages()
{
    m_colorImage = std::make_unique<Texture2D>(GL_RGBA8, m_viewportWidth, m_viewportHeight);
    m_colorImage->setMinFilter(GL_LINEAR);
    m_colorImage->setMagFilter(GL_LINEAR);

    m_framebuffer->attachTexture(GL_COLOR_ATTACHMENT0, *m_colorImage.get(), 0);
}
