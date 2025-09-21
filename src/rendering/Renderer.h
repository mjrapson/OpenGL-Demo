// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/Buffer.h"
#include "rendering/DrawCommand.h"

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

class Camera;
class Framebuffer;
class Mesh;
class Scene;
class Shader;
class Texture2D;
class TextureCubeMapArray;
class VertexLayout;

struct Container;
struct DirectionalLight;
struct PointLight;
struct Vertex;

class Renderer
{
    struct MeshBufferInfo
    {
        GLuint indexCount;
        GLuint indexBufferOffset;
        GLuint vertexBufferOffset;
        GLuint overlayVertexBufferOffset;
        GLuint overlayIndexBufferOffset;
    };

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
        void loadShaders();
        void createFramebuffers();
        void createVertexLayouts();
        
        void rebuildBuffers();
        void rebuildFramebufferImages();

        void render(const Camera& camera, const DirectionalLight& directionalLight, const std::vector<PointLight>& lights);
        void shadowMapRenderPass(const DirectionalLight& directionalLight, const std::vector<PointLight>& lights);
        void gBufferRenderPass(const Camera& camera);
        void lightingRenderPass(const DirectionalLight& directionalLight, const std::vector<PointLight>& lights);
        void overlayRenderPass(const Camera& camera);
        void present();

    private:
        std::unique_ptr<Shader> m_meshShadowShader{nullptr};
        std::unique_ptr<Shader> m_meshPointLightShadowShader{nullptr};
        std::unique_ptr<Shader> m_meshDeferredShader{nullptr};
        std::unique_ptr<Shader> m_lightingShader{nullptr};
        std::unique_ptr<Shader> m_overlayShader{nullptr};

        std::unique_ptr<Framebuffer> m_mainFramebuffer{nullptr};
        std::unique_ptr<Framebuffer> m_shadowFrameBuffer{nullptr};
        std::unique_ptr<Framebuffer> m_shadowPointLightFrameBuffer{nullptr};
        std::unique_ptr<Framebuffer> m_lightingFramebuffer{nullptr};
        std::unique_ptr<Framebuffer> m_overlayFramebuffer{nullptr};

        std::unique_ptr<VertexLayout> m_meshVertexLayout{nullptr};
        std::unique_ptr<VertexLayout> m_quadVertexLayout{nullptr};
        std::unique_ptr<VertexLayout> m_overlayVertexLayout{nullptr};

        std::unique_ptr<Texture2D> m_mainColorImage{nullptr};
        std::unique_ptr<Texture2D> m_mainNormalImage{nullptr};
        std::unique_ptr<Texture2D> m_mainDepthImage{nullptr};
        std::unique_ptr<Texture2D> m_mainPositionImage{nullptr};
        std::unique_ptr<Texture2D> m_finalColourImage{nullptr};
        std::unique_ptr<Texture2D> m_shadowMapDepthImage{nullptr};
        std::unique_ptr<TextureCubeMapArray> m_pointLightDepthImage{nullptr};

        std::unique_ptr<Buffer<Vertex>> m_meshVertexBuffer{nullptr};
        std::unique_ptr<Buffer<GLuint>> m_meshIndexBuffer{nullptr};
        std::unique_ptr<Buffer<glm::vec3>> m_overlayVertexBuffer{nullptr};
        std::unique_ptr<Buffer<GLuint>> m_overlayIndexBuffer{nullptr};
        std::unique_ptr<Buffer<float>> m_screenVertexBuffer{nullptr};

        std::unique_ptr<Container> m_assets{nullptr};

        GLuint m_width{0};
        GLuint m_height{0};

        std::unordered_map<Mesh*, MeshBufferInfo> m_meshBufferTable;
        
        std::vector<DrawCommand> m_meshDrawQueue;
        std::vector<DrawCommand> m_meshHighlightDrawQueue;
};
