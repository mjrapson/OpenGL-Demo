/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "core/Vertex.h"
#include "rendering/Buffer.h"

#include <memory>
#include <unordered_map>

class Mesh;
class VertexLayout;

class MeshBuffer
{
    public:
        explicit MeshBuffer(const std::vector<Mesh*>& meshes);
        ~MeshBuffer();

        MeshBuffer(const MeshBuffer& other) = delete;
        MeshBuffer(MeshBuffer&& other) = delete;

        MeshBuffer& operator=(const MeshBuffer& other) = delete;
        MeshBuffer& operator=(MeshBuffer&& other) = delete;   

        inline GLuint vertexOffsetOfMesh(Mesh* mesh) const;
        inline GLuint indexOffsetOfMesh(Mesh* mesh) const;

        void bindToVertexLayout(const VertexLayout& vertexLayout) const;

    private:
        std::unique_ptr<Buffer<Vertex>> m_vertexBuffer{nullptr};
        std::unique_ptr<Buffer<GLuint>> m_indexBuffer{nullptr};
        std::unordered_map<Mesh*, GLuint> m_vertexOffsets;
        std::unordered_map<Mesh*, GLuint> m_indexOffsets;
};

inline GLuint MeshBuffer::vertexOffsetOfMesh(Mesh* mesh) const
{
    return m_vertexOffsets.at(mesh);
}

inline GLuint MeshBuffer::indexOffsetOfMesh(Mesh* mesh) const
{
    return m_indexOffsets.at(mesh);
}
