// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Renderer.h"

#include "core/FileSystem.h"
#include "core/Vertex.h"
#include "data/Container.h"
#include "data/DirectionalLight.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/PointLight.h"
#include "data/Texture.h"
#include "rendering/Framebuffer.h"
#include "rendering/Shader.h"
#include "rendering/VertexLayout.h"
#include "scene/Scene.h"
#include "scene/Scene3DModel.h"
#include "scene/SceneDirectionalLightObject.h"
#include "scene/ScenePointLightObject.h"

constexpr auto maxPointLights = 32;

const auto boxIndices = std::vector<GLuint>{0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7};

const auto quadVertices =
    std::vector<float>{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
                       1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f};

 Renderer::Renderer()
 {
    loadShaders();

    createFramebuffers();

    createVertexLayouts();
 }

Renderer::~Renderer() = default;

void Renderer::setAssets(std::unique_ptr<Container> assets)
{
    m_assets = std::move(assets);

    rebuildBuffers();
}

void Renderer::resizeDisplay(GLuint width, GLuint height)
{
    m_width = width;
    m_height = height;

    rebuildFramebufferImages();
}

void Renderer::drawScene(const Scene& scene, const Camera& camera)
{
    for (const auto& [name, object] : scene.getScene3DModels())
    {
        const auto hovered = object->hovered();
        const auto selected = object->selected();

        for (const auto& instance : object->meshInstances())
        {
            auto cmd = DrawCommand{
                .instance = instance, 
                .transform = object->transformMatrix()
            };

            if(hovered || selected)
            {
                m_meshHighlightDrawQueue.push_back(cmd);
            }
            else
            {
                m_meshDrawQueue.push_back(cmd);
            }
        }
    }
}

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

void Renderer::createFramebuffers()
{
    m_shadowFrameBuffer = std::make_unique<Framebuffer>();
    m_shadowFrameBuffer->setDrawBuffer(GL_NONE);
    m_shadowFrameBuffer->setReadBuffer(GL_NONE);

    m_shadowPointLightFrameBuffer = std::make_unique<Framebuffer>();
    m_shadowPointLightFrameBuffer->setDrawBuffer(GL_NONE);
    m_shadowPointLightFrameBuffer->setReadBuffer(GL_NONE);

    m_mainFramebuffer = std::make_unique<Framebuffer>();
    m_mainFramebuffer->setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
}

void Renderer::createVertexLayouts()
{
    m_meshVertexLayout = std::make_unique<VertexLayout>();
    m_meshVertexLayout->registerAttribute(0, 3, GL_FLOAT, offsetof(Vertex, position));
    m_meshVertexLayout->registerAttribute(1, 2, GL_FLOAT, offsetof(Vertex, textureUV));
    m_meshVertexLayout->registerAttribute(2, 3, GL_FLOAT, offsetof(Vertex, normal));

    m_quadVertexLayout = std::make_unique<VertexLayout>();
    m_quadVertexLayout->registerAttribute(0, 2, GL_FLOAT, 0);
    m_quadVertexLayout->registerAttribute(1, 2, GL_FLOAT, 2 * sizeof(float));

    m_overlayVertexLayout = std::make_unique<VertexLayout>();
    m_overlayVertexLayout->registerAttribute(0, 3, GL_FLOAT, 0);
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

    m_meshVertexLayout->bindVertexBuffer(0, m_meshVertexBuffer->handle(), 0, sizeof(Vertex));
    m_meshVertexLayout->bindElementBuffer(m_meshIndexBuffer->handle());

    m_overlayVertexLayout->bindVertexBuffer(0, m_overlayVertexBuffer->handle(), 0, sizeof(Vertex));
    m_overlayVertexLayout->bindElementBuffer(m_overlayIndexBuffer->handle());

    m_quadVertexLayout->bindVertexBuffer(0, m_screenVertexBuffer->handle(), 0, 4 * sizeof(float));
}

void Renderer::rebuildFramebufferImages()
{
    constexpr auto shadowMapWidth = 2048;
    constexpr auto shadowMapHeight = 2048;

    m_shadowMapDepthImage = std::make_unique<Texture2D>(GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
    m_shadowMapDepthImage->setMinFilter(GL_NEAREST);
    m_shadowMapDepthImage->setMagFilter(GL_NEAREST);
    m_shadowMapDepthImage->setWrapS(GL_CLAMP_TO_BORDER);
    m_shadowMapDepthImage->setWrapT(GL_CLAMP_TO_BORDER);
    m_shadowMapDepthImage->setBorderColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    m_shadowMapDepthImage->setComparisonMode(GL_COMPARE_REF_TO_TEXTURE);
    m_shadowMapDepthImage->setComparisonFunction(GL_LEQUAL);

    m_pointLightDepthImage = std::make_unique<TextureCubeMapArray>(GL_DEPTH_COMPONENT32F, shadowMapWidth, shadowMapHeight, 6 * maxPointLights);
    m_pointLightDepthImage->setMinFilter(GL_NEAREST);
    m_pointLightDepthImage->setMagFilter(GL_NEAREST);
    m_pointLightDepthImage->setWrapS(GL_CLAMP_TO_EDGE);
    m_pointLightDepthImage->setWrapT(GL_CLAMP_TO_EDGE);
    m_pointLightDepthImage->setWrapR(GL_CLAMP_TO_EDGE);

    m_mainColorImage = std::make_unique<Texture2D>(GL_RGBA8, m_width, m_height);
    m_mainColorImage->setMinFilter(GL_LINEAR);
    m_mainColorImage->setMagFilter(GL_LINEAR);

    m_mainNormalImage = std::make_unique<Texture2D>(GL_RGBA16F, m_width, m_height);
    m_mainNormalImage->setMinFilter(GL_LINEAR);
    m_mainNormalImage->setMagFilter(GL_LINEAR);

    m_mainPositionImage = std::make_unique<Texture2D>(GL_RGBA32F, m_width, m_height);
    m_mainPositionImage->setMinFilter(GL_NEAREST);
    m_mainPositionImage->setMagFilter(GL_NEAREST);

    m_mainDepthImage = std::make_unique<Texture2D>(GL_DEPTH_COMPONENT24, m_width, m_height);

    m_finalColourImage = std::make_unique<Texture2D>(GL_RGBA8, m_width, m_height);
    m_finalColourImage->setMinFilter(GL_LINEAR);
    m_finalColourImage->setMagFilter(GL_LINEAR);
}
