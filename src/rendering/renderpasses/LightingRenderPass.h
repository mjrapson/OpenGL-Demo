/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/Buffer.h"
#include "rendering/RenderPass.h"

#include <functional>
#include <memory>

#include <glad/gl.h>

class Framebuffer;
class MeshBuffer;
class Shader;
class Texture2D;
class TextureCubeMapArray;
class VertexLayout;

class LightingRenderPass : public RenderPass
{
    public:
        struct Inputs
        {
            Texture2D* colorImage;
            Texture2D* normalImage;
            Texture2D* positionImage;
            Texture2D* directionalLightShadowMapImage;
            TextureCubeMapArray* pointLightShadowMapImage;
        };

         using InputBinding = std::function<Inputs(void)>; 

        LightingRenderPass();
        ~LightingRenderPass() override;   

        void execute(const std::vector<DrawCommand>& drawQueue, 
                     const SceneData& sceneData, 
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
        std::unique_ptr<Buffer<float>> m_vertexBuffer{nullptr};

        InputBinding m_inputBinding;
        
        GLuint m_viewportWidth{0};
        GLuint m_viewportHeight{0};
};
