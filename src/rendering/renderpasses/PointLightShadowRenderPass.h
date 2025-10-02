/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/RenderPass.h"

#include <memory>

class Framebuffer;
class MeshBuffer;
class Shader;
class TextureCubeMapArray;
class VertexLayout;

class PointLightShadowRenderPass : public RenderPass
{
    public:
        PointLightShadowRenderPass();
        ~PointLightShadowRenderPass() override;   

        void execute(const std::vector<DrawCommand>& drawQueue, 
                     const SceneData& sceneData, 
                     const MeshBuffer& buffer) override;

        TextureCubeMapArray* pointLightShadowMapImage() const;

    private:
        std::unique_ptr<Shader> m_shader{nullptr};
        std::unique_ptr<Framebuffer> m_framebuffer{nullptr};
        std::unique_ptr<VertexLayout> m_vertexLayout{nullptr};
        std::unique_ptr<TextureCubeMapArray> m_pointLightDepthImage{nullptr};
};
