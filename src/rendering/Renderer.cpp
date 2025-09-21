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
#include "rendering/Camera.h"
#include "rendering/Framebuffer.h"
#include "rendering/Shader.h"
#include "rendering/VertexLayout.h"
#include "scene/Scene.h"
#include "scene/Scene3DModel.h"
#include "scene/SceneDirectionalLightObject.h"
#include "scene/ScenePointLightObject.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <array>

constexpr auto maxPointLights = 32;
constexpr auto shadowMapWidth = 2048;
constexpr auto shadowMapHeight = 2048;

const auto boxIndices = std::vector<GLuint>{0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7};

const auto quadVertices =
    std::vector<float>{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
                       1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f};

struct alignas(16) TransformUbo
{
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
};

struct alignas(16) LightTransformUbo
{
        glm::mat4 lightSpaceMatrix;
        glm::mat4 model;
};

struct alignas(16) MaterialUbo
{
        int hasTexture{0};
        int _padding[3];
        glm::vec4 diffuseColor{0.0f, 0.0f, 0.0f, 1.0f};
};
static_assert(sizeof(MaterialUbo) == 32);
static_assert(offsetof(MaterialUbo, diffuseColor) == 16);

struct alignas(16) LightPositionUbo
{
        glm::vec3 lightPosition;
        float _padding;
};
static_assert(sizeof(LightPositionUbo) == 16);

struct alignas(16) FarPlaneUbo
{
        float farPlane;
        float _padding[3];
};
static_assert(sizeof(FarPlaneUbo) == 16);

struct alignas(16) DirectionalLightUbo
{
        glm::vec3 dirLightDirection;
        float _padding;
        glm::vec4 dirLightDiffuseColor;
        glm::mat4 dirLightSpaceMatrix;
};
static_assert(sizeof(DirectionalLightUbo) == 96);

struct alignas(16) AlignedPointLight
{
        glm::vec3 position;
        float _padding;
        glm::vec3 color;
        float radius{1.0f};
};
static_assert(sizeof(AlignedPointLight) == 32);

struct alignas(16) PointLightUbo
{
        int numPointLights;
        float _padding[3];
        AlignedPointLight lights[maxPointLights];
};
static_assert(sizeof(PointLightUbo) == 1040);

glm::mat4 getLightSpaceMatrix(const glm::vec3& lightDirection)
{
    // Directional lights have no position, so make a pretend one along its direction vector
    const auto normalisedDirection = glm::normalize(lightDirection);
    const auto lightPos = -normalisedDirection * 10.0f;

    // Create a projection and view from the pretend light point of view
    const auto lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 500.0f);
    const auto lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));

    return lightProjection * lightView;
}

std::array<glm::mat4, 6> getPointLightShadowTransforms(const glm::vec3& position, float farPlane)
{
    const auto shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);

    return std::array<glm::mat4, 6>{
        shadowProj * glm::lookAt(position, position + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),  // +X (Right)
        shadowProj * glm::lookAt(position, position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)), // -X (Left)
        shadowProj * glm::lookAt(position, position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),   // +Y (Up)
        shadowProj * glm::lookAt(position, position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)), // -Y (Down)
        shadowProj * glm::lookAt(position, position + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),  // +Z (Forward)
        shadowProj * glm::lookAt(position, position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))  // -Z (Backward)
    };
}

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

    auto dirLight = DirectionalLight{};
    if(scene.getDirectionalLight())
    {
        dirLight = scene.getDirectionalLight()->toLightData();
    }

    auto pointLights = std::vector<PointLight>{};
    for(const auto& light: scene.getPointLights())
    {
        pointLights.push_back(light->toLightData());
    }

    render(camera, dirLight, pointLights);
}

void Renderer::reloadShaders()
{
    loadShaders();
}

void Renderer::loadShaders()
{
    const auto shaderDir = GetShaderDir();

    m_meshShadowShader = std::make_unique<Shader>(
        shaderDir / "mesh_shadow_vertex.glsl", 
        shaderDir / "mesh_shadow_fragment.glsl"
    );
    m_meshShadowShader->registerUniformBuffer("LightTransformBlock", sizeof(LightTransformUbo), 0);

    m_meshPointLightShadowShader = std::make_unique<Shader>(
        shaderDir / "mesh_pointlight_shadow_vertex.glsl",
        shaderDir / "mesh_pointlight_shadow_fragment.glsl"
    );
    m_meshPointLightShadowShader->registerUniformBuffer("LightTransformBlock", sizeof(LightTransformUbo), 0);
    m_meshPointLightShadowShader->registerUniformBuffer("LightPositionBlock", sizeof(LightPositionUbo), 1);
    m_meshPointLightShadowShader->registerUniformBuffer("FarPlaneBlock", sizeof(FarPlaneUbo), 2);

    m_meshDeferredShader = std::make_unique<Shader>(
        shaderDir / "mesh_deferred_vertex.glsl", 
        shaderDir / "mesh_deferred_fragment.glsl"
    );
    m_meshDeferredShader->registerUniformBuffer("TransformBlock", sizeof(TransformUbo), 0);
    m_meshDeferredShader->registerTextureSampler("diffuseTexture", 1);
    m_meshDeferredShader->registerUniformBuffer("MaterialBlock", sizeof(MaterialUbo), 2);


    m_lightingShader = std::make_unique<Shader>(
        shaderDir / "lighting_deferred_vertex.glsl",
        shaderDir / "lighting_deferred_fragment.glsl"
    );
    m_lightingShader->registerTextureSampler("colorTexture", 0);
    m_lightingShader->registerTextureSampler("normalTexture", 1);
    m_lightingShader->registerTextureSampler("positionTexture", 2);
    m_lightingShader->registerTextureSampler("directionalShadowMap", 3);
    m_lightingShader->registerTextureSampler("pointLightShadowMap", 4);
    m_lightingShader->registerUniformBuffer("DirectionalLightBlock", sizeof(DirectionalLightUbo), 5);
    m_lightingShader->registerUniformBuffer("FarPlaneBlock", sizeof(FarPlaneUbo), 6);
    m_lightingShader->registerUniformBuffer("PointLightBlock", sizeof(PointLightUbo), 7);

    m_overlayShader = std::make_unique<Shader>(
        shaderDir / "overlay_vertex.glsl", 
        shaderDir / "overlay_fragment.glsl"
    );
    m_overlayShader->registerUniformBuffer("TransformBlock", sizeof(TransformUbo), 0);
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

    m_lightingFramebuffer = std::make_unique<Framebuffer>();
    m_lightingFramebuffer->setDrawBuffers({GL_COLOR_ATTACHMENT0});

    m_overlayFramebuffer = std::make_unique<Framebuffer>();
    m_overlayFramebuffer->setDrawBuffer({GL_COLOR_ATTACHMENT0});
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

    m_mainFramebuffer->attachTexture(GL_COLOR_ATTACHMENT0, *m_mainColorImage.get(), 0);
    m_mainFramebuffer->attachTexture(GL_COLOR_ATTACHMENT1, *m_mainNormalImage.get(), 0);
    m_mainFramebuffer->attachTexture(GL_COLOR_ATTACHMENT2, *m_mainPositionImage.get(), 0);
    m_mainFramebuffer->attachTexture(GL_DEPTH_ATTACHMENT, *m_mainDepthImage.get(), 0);

    m_finalColourImage = std::make_unique<Texture2D>(GL_RGBA8, m_width, m_height);
    m_finalColourImage->setMinFilter(GL_LINEAR);
    m_finalColourImage->setMagFilter(GL_LINEAR);

    m_lightingFramebuffer->attachTexture(GL_COLOR_ATTACHMENT0, *m_finalColourImage.get(), 0);
    m_overlayFramebuffer->attachTexture(GL_COLOR_ATTACHMENT0, *m_finalColourImage.get(), 0);
}

void Renderer::render(const Camera& camera, const DirectionalLight& directionalLight, const std::vector<PointLight>& lights)
{
    shadowMapRenderPass(directionalLight, lights);

    gBufferRenderPass(camera);

    lightingRenderPass(directionalLight, lights);

    overlayRenderPass(camera);

    present();

    m_meshDrawQueue.clear();
    m_meshHighlightDrawQueue.clear();
}

void Renderer::shadowMapRenderPass(const DirectionalLight& directionalLight, const std::vector<PointLight>& lights)
{
    m_shadowFrameBuffer->bind();
    m_meshShadowShader->bind();
    m_shadowFrameBuffer->attachTexture(GL_DEPTH_ATTACHMENT, *m_shadowMapDepthImage.get(), 0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    m_meshVertexLayout->bind();

    auto lightTransformUbo = LightTransformUbo{};
    lightTransformUbo.lightSpaceMatrix = getLightSpaceMatrix(directionalLight.direction);

    for (const auto& drawCommand : m_meshDrawQueue)
    {
        if (!m_meshBufferTable.contains(drawCommand.instance.mesh))
        {
            continue;
        }

        const auto& data = m_meshBufferTable.at(drawCommand.instance.mesh);

        lightTransformUbo.model = drawCommand.transform;

        m_meshShadowShader->writeUniformData("LightTransformBlock", sizeof(LightTransformUbo), &lightTransformUbo);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(data.indexCount),
            GL_UNSIGNED_INT,
            reinterpret_cast<const void*>(data.indexBufferOffset * sizeof(unsigned int)),
            data.vertexBufferOffset);
    }

    m_shadowPointLightFrameBuffer->bind();
    m_meshPointLightShadowShader->bind();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    const float farPlane = 50.0f;

    auto lightIndex = 0;
    for (const auto& light : lights)
    {
        const auto lightTransforms = getPointLightShadowTransforms(light.position, farPlane);

        for (auto i = 0; i < 6; ++i)
        {
            m_shadowPointLightFrameBuffer->attachTextureLayer(GL_DEPTH_ATTACHMENT, *m_pointLightDepthImage.get(), 0, (6 * lightIndex) + i);
            glViewport(0, 0, shadowMapWidth, shadowMapHeight);
            glClear(GL_DEPTH_BUFFER_BIT);


            const auto lightPosUbo = LightPositionUbo{.lightPosition = glm::vec3{light.position.x, light.position.y, light.position.z}};
            m_meshPointLightShadowShader->writeUniformData("LightPositionBlock", sizeof(LightPositionUbo), &lightPosUbo);

            const auto farPlaneUbo = FarPlaneUbo{.farPlane = farPlane};
            m_meshPointLightShadowShader->writeUniformData("FarPlaneBlock", sizeof(FarPlaneUbo), &farPlaneUbo);

            for (const auto& drawCommand : m_meshDrawQueue)
            {
                if (!m_meshBufferTable.contains(drawCommand.instance.mesh))
                {
                    continue;
                }

                const auto& data = m_meshBufferTable.at(drawCommand.instance.mesh);

                auto pointLightTransformUbo = LightTransformUbo{};
                pointLightTransformUbo.lightSpaceMatrix = lightTransforms.at(i);
                pointLightTransformUbo.model = drawCommand.transform;
                m_meshPointLightShadowShader->writeUniformData("LightTransformBlock", sizeof(LightTransformUbo), &pointLightTransformUbo);

                glDrawElementsBaseVertex(
                    GL_TRIANGLES,
                    static_cast<GLsizei>(data.indexCount),
                    GL_UNSIGNED_INT,
                    reinterpret_cast<const void*>(data.indexBufferOffset * sizeof(GLuint)),
                    data.vertexBufferOffset);
            }
            
        }
        ++lightIndex;
    }
}

void Renderer::gBufferRenderPass(const Camera& camera)
{
    m_mainFramebuffer->bind();
    m_meshDeferredShader->bind();
    m_meshVertexLayout->bind();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, m_width, m_height);

    for (const auto& drawCommand : m_meshDrawQueue)
    {
        if (!m_meshBufferTable.contains(drawCommand.instance.mesh))
        {
            continue;
        }

        const auto& data = m_meshBufferTable.at(drawCommand.instance.mesh);
        const auto* material = drawCommand.instance.material;

        auto transformUbo = TransformUbo{};
        transformUbo.projection = camera.projection();
        transformUbo.view = camera.view();
        transformUbo.model = drawCommand.transform;

        auto materialUbo = MaterialUbo{};
        materialUbo.diffuseColor = glm::vec4{material->diffuse, 1.0f};
        if (material->diffuseTexture)
        {
            materialUbo.hasTexture = 1;
            m_meshDeferredShader->bindTexture("diffuseTexture", material->diffuseTexture.value());
        }
        else
        {
            materialUbo.hasTexture = 0;
        }
        m_meshDeferredShader->writeUniformData("TransformBlock", sizeof(TransformUbo), &transformUbo);
        m_meshDeferredShader->writeUniformData("MaterialBlock", sizeof(MaterialUbo), &materialUbo);

        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            static_cast<GLsizei>(data.indexCount),
            GL_UNSIGNED_INT,
            reinterpret_cast<const void*>(data.indexBufferOffset * sizeof(GLuint)),
            data.vertexBufferOffset);
    }
}

void Renderer::lightingRenderPass(const DirectionalLight& directionalLight, const std::vector<PointLight>& lights)
{
    m_lightingFramebuffer->bind();
    m_lightingShader->bind();
    m_quadVertexLayout->bind();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_lightingShader->bindTexture("colorTexture", m_mainColorImage.get());
    m_lightingShader->bindTexture("normalTexture", m_mainNormalImage.get());
    m_lightingShader->bindTexture("positionTexture", m_mainPositionImage.get());
    m_lightingShader->bindTexture("directionalShadowMap", m_shadowMapDepthImage.get());
    m_lightingShader->bindTexture("pointLightShadowMap", m_pointLightDepthImage.get());

    auto directionalLightUbo = DirectionalLightUbo{};
    directionalLightUbo.dirLightDiffuseColor = glm::vec4{directionalLight.color, 1.0f};
    directionalLightUbo.dirLightDirection = directionalLight.direction;
    directionalLightUbo.dirLightSpaceMatrix = getLightSpaceMatrix(directionalLight.direction);
    m_lightingShader->writeUniformData("DirectionalLightBlock", sizeof(DirectionalLightUbo), &directionalLightUbo);

    const auto farPlaneUbo = FarPlaneUbo{.farPlane = 50.0f};
    m_lightingShader->writeUniformData("FarPlaneBlock", sizeof(FarPlaneUbo), &farPlaneUbo);

    auto pointLightUbo = PointLightUbo{};
    pointLightUbo.numPointLights = static_cast<int>(lights.size());

    for (size_t i = 0; i < lights.size() && i < maxPointLights; i++)
    {
        pointLightUbo.lights[i].position = lights[i].position;
        pointLightUbo.lights[i].color = lights[i].color;
        pointLightUbo.lights[i].radius = lights[i].radius;
    }

    m_lightingShader->writeUniformData("PointLightBlock", sizeof(PointLightUbo), &pointLightUbo);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::overlayRenderPass(const Camera& camera)
{
    m_overlayFramebuffer->bind();
    m_overlayShader->bind();
    m_overlayVertexLayout->bind();
    
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    for (auto& drawCommand : m_meshHighlightDrawQueue)
    {
        if (!m_meshBufferTable.contains(drawCommand.instance.mesh))
        {
            continue;
        }

        const auto& data = m_meshBufferTable.at(drawCommand.instance.mesh);

        auto transformUbo = TransformUbo{};
        transformUbo.projection = camera.projection();
        transformUbo.view = camera.view();
        transformUbo.model = drawCommand.transform;
        m_overlayShader->writeUniformData("TransformBlock", sizeof(TransformUbo), &transformUbo);

        glDrawElementsBaseVertex(
            GL_LINES,
            static_cast<GLsizei>(24),
            GL_UNSIGNED_INT,
            0,
            data.overlayVertexBufferOffset);
    }
}

void Renderer::present()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_overlayFramebuffer->handle());

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(
        0,
        0,
        m_width,
        m_height,
        0,
        0,
        m_width,
        m_height,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
