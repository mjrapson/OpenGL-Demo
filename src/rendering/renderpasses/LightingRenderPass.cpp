/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "LightingRenderPass.h"

#include "core/FileSystem.h"
#include "data/DirectionalLight.h"
#include "data/PointLight.h"
#include "data/Texture.h"
#include "rendering/Framebuffer.h"
#include "rendering/LightTransform.h"
#include "rendering/Shader.h"
#include "rendering/VertexLayout.h"

#include <glm/glm.hpp>

constexpr auto maxPointLights = 8;

const auto quadVertices =
    std::vector<float>{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
                       1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f};


struct alignas(16) DirectionalLightUbo
{
        glm::vec3 dirLightDirection;
        float _padding;
        glm::vec4 dirLightDiffuseColor;
        glm::mat4 dirLightSpaceMatrix;
};

struct alignas(16) FarPlaneUbo
{
        float farPlane;
        float _padding[3];
};

struct alignas(16) AlignedPointLight
{
        glm::vec3 position;
        float _padding;
        glm::vec3 color;
        float radius{1.0f};
};

struct alignas(16) PointLightUbo
{
        int numPointLights;
        float _padding[3];
        AlignedPointLight lights[maxPointLights];
};

LightingRenderPass::LightingRenderPass()
    : RenderPass()
{
    const auto shaderDir = GetShaderDir();
    const auto vsPath = shaderDir / "lighting_deferred_vertex.glsl";
    const auto fsPath = shaderDir / "lighting_deferred_fragment.glsl";

    m_shader = std::make_unique<Shader>(vsPath, fsPath);
    m_shader->registerTextureSampler("colorTexture", 0);
    m_shader->registerTextureSampler("normalTexture", 1);
    m_shader->registerTextureSampler("positionTexture", 2);
    m_shader->registerTextureSampler("directionalShadowMap", 3);
    m_shader->registerTextureSampler("pointLightShadowMap", 4);
    m_shader->registerUniformBuffer("DirectionalLightBlock", sizeof(DirectionalLightUbo), 5);
    m_shader->registerUniformBuffer("FarPlaneBlock", sizeof(FarPlaneUbo), 6);
    m_shader->registerUniformBuffer("PointLightBlock", sizeof(PointLightUbo), 7);

    m_framebuffer = std::make_unique<Framebuffer>();
    m_framebuffer->setDrawBuffers({GL_COLOR_ATTACHMENT0});

    m_vertexLayout = std::make_unique<VertexLayout>();
    m_vertexLayout->registerAttribute(0, 2, GL_FLOAT, 0);
    m_vertexLayout->registerAttribute(1, 2, GL_FLOAT, 2 * sizeof(float));

    m_vertexBuffer = std::make_unique<Buffer<float>>(quadVertices);
    m_vertexLayout->bindVertexBuffer(0, m_vertexBuffer->handle(), 0, 4 * sizeof(float));
}

LightingRenderPass::~LightingRenderPass() = default;

void LightingRenderPass::execute(const std::vector<DrawCommand>& drawQueue, 
                                 const Camera& camera,
                                 const DirectionalLight& directionalLight,
                                 const std::vector<PointLight>& pointLights,
                                 const MeshBuffer& buffer)
{
    m_shader->bind();
    m_framebuffer->bind();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
    m_vertexLayout->bind();

    const auto inputs = m_inputBinding();

    m_shader->bindTexture("colorTexture", inputs.colorImage);
    m_shader->bindTexture("normalTexture", inputs.normalImage);
    m_shader->bindTexture("positionTexture", inputs.positionImage);
    m_shader->bindTexture("directionalShadowMap", inputs.directionalLightShadowMapImage);
    m_shader->bindTexture("pointLightShadowMap", inputs.pointLightShadowMapImage);

    auto directionalLightUbo = DirectionalLightUbo{};
    directionalLightUbo.dirLightDiffuseColor = glm::vec4{directionalLight.color, 1.0f};
    directionalLightUbo.dirLightDirection = directionalLight.direction;
    directionalLightUbo.dirLightSpaceMatrix = getLightSpaceMatrix(directionalLight.direction);
    m_shader->writeUniformData("DirectionalLightBlock", sizeof(DirectionalLightUbo), &directionalLightUbo);

    const auto farPlaneUbo = FarPlaneUbo{.farPlane = 50.0f};
    m_shader->writeUniformData("FarPlaneBlock", sizeof(FarPlaneUbo), &farPlaneUbo);

    auto pointLightUbo = PointLightUbo{};
    pointLightUbo.numPointLights = static_cast<int>(pointLights.size());

    for (size_t i = 0; i < pointLights.size() && i < maxPointLights; i++)
    {
        pointLightUbo.lights[i].position = pointLights[i].position;
        pointLightUbo.lights[i].color = pointLights[i].color;
        pointLightUbo.lights[i].radius = pointLights[i].radius;
    }

    m_shader->writeUniformData("PointLightBlock", sizeof(PointLightUbo), &pointLightUbo);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void LightingRenderPass::onViewportResize(GLuint width, GLuint height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;

    rebuildImages();
}

void LightingRenderPass::setInputBinding(InputBinding binding)
{
    m_inputBinding = std::move(binding);
}

Texture2D* LightingRenderPass::colorImage() const
{
    return m_colorImage.get();
}

GLuint LightingRenderPass::framebufferHandle() const
{
    return m_framebuffer->handle();
}

void LightingRenderPass::rebuildImages()
{
    m_colorImage = std::make_unique<Texture2D>(GL_RGBA8, m_viewportWidth, m_viewportHeight);
    m_colorImage->setMinFilter(GL_LINEAR);
    m_colorImage->setMagFilter(GL_LINEAR);

    m_framebuffer->attachTexture(GL_COLOR_ATTACHMENT0, *m_colorImage.get(), 0);
}
