// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/Buffer.h"
#include "rendering/DrawCommand.h"

#include <memory>

class DirectionalShadowRenderPass;
class GBufferRenderPass;
class LightingRenderPass;
class MeshBuffer;
class PointLightShadowRenderPass;

struct Container;
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

        void setAssets(std::unique_ptr<Container> assets);

        void resizeDisplay(GLuint width, GLuint height);

        void render(const std::vector<DrawCommand>& commands, const SceneData& sceneData) const;

    private:
        void rebuildBuffers();
        void present() const;

    private:
        std::unique_ptr<DirectionalShadowRenderPass> m_directionalShadowRenderPass{nullptr};
        std::unique_ptr<PointLightShadowRenderPass> m_pointLightShadowRenderPass{nullptr};
        std::unique_ptr<GBufferRenderPass> m_gbufferRenderPass{nullptr};
        std::unique_ptr<LightingRenderPass> m_lightingRenderPass{nullptr};
        
        std::unique_ptr<MeshBuffer> m_meshBuffer{nullptr};

        std::unique_ptr<Container> m_assets{nullptr};

        GLuint m_width{0};
        GLuint m_height{0};
};
