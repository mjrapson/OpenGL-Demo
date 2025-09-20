// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Renderer.h"

#include "core/FileSystem.h"
#include "core/Vertex.h"
#include "data/Container.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/Texture.h"
#include "rendering/Shader.h"

const auto boxIndices = std::vector<GLuint>{0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7};

const auto quadVertices =
    std::vector<float>{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
                       1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f};

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

void Renderer::setAssets(std::unique_ptr<Container> assets)
{
    m_assets = std::move(assets);

    rebuildBuffers();
}

void Renderer::resizeDisplay(GLuint width, GLuint height)
{
    m_width = width;
    m_height = height;

    rebuildFramebuffers();
}

void Renderer::rebuildBuffers()
{
    m_meshBufferTable.clear();

    auto vertexBufferOffset = GLuint{0};
    auto indexBufferOffset = GLuint{0};
    auto overlayVertexBufferOffset = GLuint{0};

    auto vertices = std::vector<Vertex>{};
    auto indices = std::vector<GLuint>{};
    auto bbVertices = std::vector<glm::vec3>{};

    for (const auto& [name, mesh] : m_assets->meshes)
    {
        const auto meshVertices = mesh->vertices();
        const auto meshIndices = mesh->indices();
        const auto meshBBVertices = mesh->boundingBox().vertices();

        const auto vertexCount = static_cast<GLuint>(meshVertices.size());
        const auto indexCount = static_cast<GLuint>(meshIndices.size());
        const auto bbVertexCount = static_cast<GLuint>(meshBBVertices.size());

        m_meshBufferTable[mesh.get()] = MeshBufferInfo{
            .indexCount = indexCount,
            .indexBufferOffset = indexBufferOffset,
            .vertexBufferOffset = vertexBufferOffset,
            .overlayVertexBufferOffset = overlayVertexBufferOffset,
            .overlayIndexBufferOffset = 0,
        };

        vertices.insert(vertices.end(),meshVertices.begin(), meshVertices.end());
        indices.insert(indices.end(), meshIndices.begin(), meshIndices.end());
        bbVertices.insert(bbVertices.end(), meshBBVertices.begin(), meshBBVertices.end());

        vertexBufferOffset += vertexCount;
        indexBufferOffset += indexCount;
        overlayVertexBufferOffset += bbVertexCount;
    }

    m_meshVertexBuffer = std::make_unique<Buffer<Vertex>>(vertices);
    m_meshIndexBuffer = std::make_unique<Buffer<GLuint>>(indices);
    m_overlayVertexBuffer = std::make_unique<Buffer<glm::vec3>>(bbVertices);
    m_overlayIndexBuffer = std::make_unique<Buffer<GLuint>>(boxIndices);
    m_screenVertexBuffer = std::make_unique<Buffer<float>>(quadVertices);
}

void Renderer::rebuildFramebuffers()
{

}