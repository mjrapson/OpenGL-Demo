// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Renderer.h"

#include "data/AssetDatabase.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/Texture.h"
#include "rendering/Camera.h"
#include "rendering/MeshBuffer.h"

constexpr auto maxPointLights = 8;

void GLAPIENTRY MessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(
        stderr,
        "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type,
        severity,
        message);
}
Renderer::Renderer()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 0, NULL, GL_FALSE);
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, 0, NULL, GL_FALSE);

    m_lightingRenderPass.setInputBinding(
        [
            gbuffer = &m_gbufferRenderPass,
            dirShadow = &m_directionalShadowRenderPass,
            plShadow = &m_pointLightShadowRenderPass
        ]
        ()
        {
            auto inputs = LightingRenderPass::Inputs{};
            inputs.colorImage = gbuffer->colorImage();
            inputs.normalImage = gbuffer->normalImage();
            inputs.positionImage = gbuffer->positionImage();
            inputs.directionalLightShadowMapImage = dirShadow->directionalLightShadowMapImage();
            inputs.pointLightShadowMapImage = plShadow->pointLightShadowMapImage();
            return inputs;
        }
    );

    m_skyboxRenderPass.setInputBinding(
        [
            lightingPass = &m_lightingRenderPass
        ]
        ()
        {
            auto inputs = SkyboxRenderPass::Inputs{};
            inputs.targetImage = lightingPass->colorImage();
            return inputs;
        }
    );
}

Renderer::~Renderer() = default;

void Renderer::setAssets(const AssetDatabase& assetDb)
{
    m_meshBuffer = std::make_unique<MeshBuffer>(assetDb.meshContainer());
}

void Renderer::resizeDisplay(GLuint width, GLuint height)
{
    m_width = width;
    m_height = height;

    m_gbufferRenderPass.onViewportResize(width, height);
    m_lightingRenderPass.onViewportResize(width, height);
    m_skyboxRenderPass.onViewportResize(width, height);
}

void Renderer::setDirectionalLight(const DirectionalLight& light)
{
    m_directionalLight = light;
}

void Renderer::addPointLight(const PointLight& light)
{
    m_pointLights.push_back(light);
}

void Renderer::queueDrawCommand(const DrawCommand& command)
{
    m_drawCommands.push_back(command);
}

void Renderer::render(const Camera& camera)
{
    m_directionalShadowRenderPass.execute(m_drawCommands, camera, m_directionalLight, m_pointLights, *m_meshBuffer);
    m_pointLightShadowRenderPass.execute(m_drawCommands, camera, m_directionalLight, m_pointLights, *m_meshBuffer);
    m_gbufferRenderPass.execute(m_drawCommands, camera, m_directionalLight, m_pointLights, *m_meshBuffer);
    m_lightingRenderPass.execute(m_drawCommands, camera, m_directionalLight, m_pointLights, *m_meshBuffer);

    if(camera.skyboxTexture)
    {
        m_skyboxRenderPass.execute(m_drawCommands, camera, m_directionalLight, m_pointLights, *m_meshBuffer);
    }

    present();
}

void Renderer::beginFrame()
{
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::endFrame()
{
    m_drawCommands.clear();
    m_pointLights.clear();
}

void Renderer::present() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_lightingRenderPass.framebufferHandle());

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(
        0,
        0,
        m_width,
        m_height,
        0,
        0,
        m_width,
        m_height,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
