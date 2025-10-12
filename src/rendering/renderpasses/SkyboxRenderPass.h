/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "core/Vertex.h"
#include "rendering/Buffer.h"
#include "rendering/RenderPass.h"

#include <glad/gl.h>

#include <functional>
#include <memory>

class Framebuffer;
class MeshBuffer;
class Shader;
class Texture2D;
class VertexLayout;

class SkyboxRenderPass : public RenderPass
{
    public:
        struct Inputs
        {
            Texture2D* targetImage;
        };

        using InputBinding = std::function<Inputs(void)>; 

        SkyboxRenderPass();
        ~SkyboxRenderPass() override;   

        void execute(const std::vector<DrawCommand>& drawQueue, 
                     const Camera& camera,
                     const DirectionalLight& directionalLight,
                     const std::vector<PointLight>& pointLights,
                     const MeshBuffer& buffer) override;

        void onViewportResize(GLuint width, GLuint height); 

        void setInputBinding(InputBinding binding);

        Texture2D* colorImage() const;

        GLuint framebufferHandle() const;

    private:
        void rebuildImages();

    private:
        std::unique_ptr<Shader> m_shader{nullptr};
        std::unique_ptr<Framebuffer> m_framebuffer{nullptr};
        std::unique_ptr<VertexLayout> m_vertexLayout{nullptr};
        std::unique_ptr<Texture2D> m_colorImage{nullptr};
        std::unique_ptr<Mesh> m_skyboxSphereMesh{nullptr};
        std::unique_ptr<Buffer<Vertex>> m_vertexBuffer{nullptr};
        std::unique_ptr<Buffer<GLuint>> m_indexBuffer{nullptr};

        InputBinding m_inputBinding;

        GLuint m_viewportWidth{0};
        GLuint m_viewportHeight{0};
};
