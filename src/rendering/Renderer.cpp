// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Renderer.h"

#include "core/FileSystem.h"
#include "rendering/Shader.h"

 Renderer::Renderer()
 {
    loadShaders();
 }

Renderer::~Renderer() = default;

void Renderer::loadShaders()
{
    const auto shaderDir = GetShaderDir();

    m_meshShadowShader = std::make_unique<Shader>(
        shaderDir / "mesh_shadow_vertex.glsl", 
        shaderDir / "mesh_shadow_fragment.glsl"
    );

    m_meshPointLightShadowShader = std::make_unique<Shader>(
        shaderDir / "mesh_pointlight_shadow_vertex.glsl",
        shaderDir / "mesh_pointlight_shadow_fragment.glsl"
    );

    m_meshDeferredShader = std::make_unique<Shader>(
        shaderDir / "mesh_deferred_vertex.glsl", 
        shaderDir / "mesh_deferred_fragment.glsl"
    );

    m_lightingShader = std::make_unique<Shader>(
        shaderDir / "lighting_deferred_vertex.glsl",
        shaderDir / "lighting_deferred_fragment.glsl");

    m_overlayShader = std::make_unique<Shader>(
        shaderDir / "overlay_vertex.glsl", 
        shaderDir / "overlay_fragment.glsl"
    );
}