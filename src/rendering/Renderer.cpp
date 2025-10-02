// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Renderer.h"

#include "data/Container.h"
#include "data/DirectionalLight.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/Texture.h"
#include "rendering/Camera.h"
#include "rendering/MeshBuffer.h"
#include "rendering/SceneData.h"
#include "rendering/renderpasses/DirectionalShadowRenderPass.h"
#include "rendering/renderpasses/GBufferRenderPass.h"
#include "rendering/renderpasses/LightingRenderPass.h"
#include "rendering/renderpasses/PointLightShadowRenderPass.h"
#include "scene/Scene.h"
#include "scene/Scene3DModel.h"
#include "scene/SceneDirectionalLightObject.h"
#include "scene/ScenePointLightObject.h"

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

    m_directionalShadowRenderPass = std::make_unique<DirectionalShadowRenderPass>();
    m_pointLightShadowRenderPass = std::make_unique<PointLightShadowRenderPass>();
    m_gbufferRenderPass = std::make_unique<GBufferRenderPass>();
    m_lightingRenderPass = std::make_unique<LightingRenderPass>();

    m_lightingRenderPass->setInputBinding([
        gbuffer = m_gbufferRenderPass.get(),
        dirShadow = m_directionalShadowRenderPass.get(),
        plShadow = m_pointLightShadowRenderPass.get()]
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
}

Renderer::~Renderer() = default;

void Renderer::setAssets(std::unique_ptr<Container> assets)
{
    m_assets = std::move(assets);

    rebuildBuffers();
}

void Renderer::resizeDisplay(GLuint width, GLuint height)
{
    m_width = width;
    m_height = height;

    m_gbufferRenderPass->onViewportResize(width, height);
    m_lightingRenderPass->onViewportResize(width, height);
}

void Renderer::drawScene(const Scene& scene, const Camera& camera)
{
    for (const auto& [name, object] : scene.getScene3DModels())
    {
        for (const auto& instance : object->meshInstances())
        {
            auto cmd = DrawCommand{};
            cmd.instance = instance;
            cmd.transform = object->transformMatrix();

            m_meshDrawQueue.push_back(cmd);
        }
    }

    auto dirLight = DirectionalLight{};
    if(scene.getDirectionalLight())
    {
        dirLight = scene.getDirectionalLight()->toLightData();
    }

    auto pointLights = std::vector<PointLight>{};
    for(const auto& light: scene.getPointLights())
    {
        pointLights.push_back(light->toLightData());
    }

    render(camera, dirLight, pointLights);
}

void Renderer::rebuildBuffers()
{
    auto meshes = std::vector<Mesh*>{};
    meshes.reserve(m_assets->meshes.size());

    for (const auto& [name, mesh] : m_assets->meshes)
    {
        meshes.push_back(mesh.get());
    }

    m_meshBuffer = std::make_unique<MeshBuffer>(meshes);
}

void Renderer::render(const Camera& camera, const DirectionalLight& directionalLight, const std::vector<PointLight>& lights)
{
    auto sceneData = SceneData{
        .camera = &camera, 
        .directionalLight = &directionalLight,
        .pointLights = lights
    };

    m_directionalShadowRenderPass->execute(m_meshDrawQueue, sceneData, *m_meshBuffer);
    m_pointLightShadowRenderPass->execute(m_meshDrawQueue, sceneData, *m_meshBuffer);
    m_gbufferRenderPass->execute(m_meshDrawQueue, sceneData, *m_meshBuffer);
    m_lightingRenderPass->execute(m_meshDrawQueue, sceneData, *m_meshBuffer);

    present();

    m_meshDrawQueue.clear();
}

void Renderer::present()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_lightingRenderPass->framebufferHandle());

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
