/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/RenderPass.h"

#include <memory>

#include <glad/gl.h>

class Framebuffer;
class MeshBuffer;
class Shader;
class Texture2D;
class VertexLayout;

class GBufferRenderPass : public RenderPass
{
    public:
        GBufferRenderPass();
        ~GBufferRenderPass() override;   

        void execute(const std::vector<DrawCommand>& drawQueue, 
                     const Camera& camera,
                     const DirectionalLight& directionalLight,
                     const std::vector<PointLight>& pointLights,
                     const MeshBuffer& buffer) override;

        void onViewportResize(GLuint width, GLuint height);

        Texture2D* colorImage() const;
        Texture2D* normalImage() const;
        Texture2D* positionImage() const;
        Texture2D* depthImage() const;

    private:
        void rebuildImages();

    private:
        std::unique_ptr<Shader> m_shader{nullptr};
        std::unique_ptr<Framebuffer> m_framebuffer{nullptr};
        std::unique_ptr<VertexLayout> m_vertexLayout{nullptr};
        std::unique_ptr<Texture2D> m_colorImage{nullptr};
        std::unique_ptr<Texture2D> m_normalImage{nullptr};
        std::unique_ptr<Texture2D> m_positionImage{nullptr};
        std::unique_ptr<Texture2D> m_depthImage{nullptr};
        
        GLuint m_viewportWidth{0};
        GLuint m_viewportHeight{0};
        float m_aspectRatio{0.0f};
};
