// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/DirectionalLight.h"
#include "data/PointLight.h"
#include "rendering/Buffer.h"
#include "rendering/DrawCommand.h"
#include "rendering/renderpasses/DirectionalShadowRenderPass.h"
#include "rendering/renderpasses/GBufferRenderPass.h"
#include "rendering/renderpasses/LightingRenderPass.h"
#include "rendering/renderpasses/PointLightShadowRenderPass.h"
#include "rendering/renderpasses/SkyboxRenderPass.h"

#include <memory>
#include <vector>

class AssetDatabase;
class MeshBuffer;

struct Camera;
struct SceneData;

class Renderer
{
    public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;

        Renderer& operator=(const Renderer& other) = delete;
        Renderer& operator=(Renderer&& other) = delete;   

        void setAssets(const AssetDatabase& assetDb);

        void resizeDisplay(GLuint width, GLuint height);

        void setDirectionalLight(const DirectionalLight& light);
        void addPointLight(const PointLight& light);
        void queueDrawCommand(const DrawCommand& command);

        void render(const Camera& camera);

        void beginFrame();
        void endFrame();

    private:
        void rebuildBuffers();
        void present() const;

    private:
        SkyboxRenderPass m_skyboxRenderPass;
        DirectionalShadowRenderPass m_directionalShadowRenderPass;
        PointLightShadowRenderPass m_pointLightShadowRenderPass;
        GBufferRenderPass m_gbufferRenderPass;
        LightingRenderPass m_lightingRenderPass;
        
        std::unique_ptr<MeshBuffer> m_meshBuffer{nullptr};
        DirectionalLight m_directionalLight;
        std::vector<PointLight> m_pointLights;
        std::vector<DrawCommand> m_drawCommands;

        GLuint m_width{0};
        GLuint m_height{0};
};
