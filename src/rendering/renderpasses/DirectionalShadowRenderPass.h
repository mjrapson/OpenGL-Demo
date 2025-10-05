/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/RenderPass.h"

#include <memory>

class Framebuffer;
class MeshBuffer;
class Shader;
class Texture2D;
class VertexLayout;

class DirectionalShadowRenderPass : public RenderPass
{
    public:
        DirectionalShadowRenderPass();
        ~DirectionalShadowRenderPass() override;   

        void execute(const std::vector<DrawCommand>& drawQueue, 
                     const Camera& camera,
                     const DirectionalLight& directionalLight,
                     const std::vector<PointLight>& pointLights,
                     const MeshBuffer& buffer) override;

        Texture2D* directionalLightShadowMapImage() const;

    private:
        std::unique_ptr<Shader> m_shader{nullptr};
        std::unique_ptr<Framebuffer> m_framebuffer{nullptr};
        std::unique_ptr<VertexLayout> m_vertexLayout{nullptr};
        std::unique_ptr<Texture2D> m_shadowMapDepthImage{nullptr};
};
