// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/Buffer.h"
#include "rendering/DrawCommand.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Camera;
class DirectionalShadowRenderPass;
class Framebuffer;
class GBufferRenderPass;
class LightingRenderPass;
class Mesh;
class MeshBuffer;
class PointLightShadowRenderPass;
class Scene;
class VertexLayout;

struct Container;
struct DirectionalLight;
struct PointLight;
struct Vertex;

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

        void drawScene(const Scene& scene, const Camera& camera);

    private:
        void rebuildBuffers();

        void render(const Camera& camera, const DirectionalLight& directionalLight, const std::vector<PointLight>& lights);
        void lightingRenderPass(const DirectionalLight& directionalLight, const std::vector<PointLight>& lights);
        void present();

    private:
        std::unique_ptr<DirectionalShadowRenderPass> m_directionalShadowRenderPass{nullptr};
        std::unique_ptr<PointLightShadowRenderPass> m_pointLightShadowRenderPass{nullptr};
        std::unique_ptr<GBufferRenderPass> m_gbufferRenderPass{nullptr};
        std::unique_ptr<LightingRenderPass> m_lightingRenderPass{nullptr};
        
        std::unique_ptr<MeshBuffer> m_meshBuffer{nullptr};

        std::unique_ptr<Container> m_assets{nullptr};

        GLuint m_width{0};
        GLuint m_height{0};
        
        std::vector<DrawCommand> m_meshDrawQueue;
};
