// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <memory>

class Shader;

class Renderer
{
    public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;

        Renderer& operator=(const Renderer& other) = delete;
        Renderer& operator=(Renderer&& other) = delete;

        void loadShaders();
    private:
        std::unique_ptr<Shader> m_meshShadowShader{nullptr};
        std::unique_ptr<Shader> m_meshPointLightShadowShader{nullptr};
        std::unique_ptr<Shader> m_meshDeferredShader{nullptr};
        std::unique_ptr<Shader> m_lightingShader{nullptr};
        std::unique_ptr<Shader> m_overlayShader{nullptr};
};