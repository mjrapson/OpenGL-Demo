// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/Buffer.h"

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>

class Mesh;
class Shader;

struct Container;
struct Vertex;

class Renderer
{
    struct MeshBufferInfo
    {
        GLuint indexCount;
        GLuint indexBufferOffset;
        GLuint vertexBufferOffset;
        GLuint overlayVertexBufferOffset;
        GLuint overlayIndexBufferOffset;
    };

    public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;

        Renderer& operator=(const Renderer& other) = delete;
        Renderer& operator=(Renderer&& other) = delete;

        void loadShaders();

        void setAssets(std::unique_ptr<Container> assets);

        void resizeDisplay(GLuint width, GLuint height);

    private:
        void rebuildBuffers();
        void rebuildFramebuffers();

    private:
        std::unique_ptr<Shader> m_meshShadowShader{nullptr};
        std::unique_ptr<Shader> m_meshPointLightShadowShader{nullptr};
        std::unique_ptr<Shader> m_meshDeferredShader{nullptr};
        std::unique_ptr<Shader> m_lightingShader{nullptr};
        std::unique_ptr<Shader> m_overlayShader{nullptr};

        std::unique_ptr<Buffer<Vertex>> m_meshVertexBuffer{nullptr};
        std::unique_ptr<Buffer<GLuint>> m_meshIndexBuffer{nullptr};
        std::unique_ptr<Buffer<glm::vec3>> m_overlayVertexBuffer{nullptr};
        std::unique_ptr<Buffer<GLuint>> m_overlayIndexBuffer{nullptr};
        std::unique_ptr<Buffer<float>> m_screenVertexBuffer{nullptr};

        std::unique_ptr<Container> m_assets{nullptr};

        GLuint m_width;
        GLuint m_height;

        std::unordered_map<Mesh*, MeshBufferInfo> m_meshBufferTable;
};