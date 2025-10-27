/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "MeshBuffer.h"

#include "data/Mesh.h"
#include "data/Prefab.h"
#include "rendering/VertexLayout.h"

MeshBuffer::MeshBuffer(const std::vector<Mesh*>& meshes)
{
    auto vertexBufferOffset = GLuint{0};
    auto indexBufferOffset = GLuint{0};

    auto vertices = std::vector<Vertex>{};
    auto indices = std::vector<GLuint>{};

    for (const auto& mesh : meshes)
    {
        const auto vertexCount = static_cast<GLuint>(mesh->vertices.size());
        const auto indexCount = static_cast<GLuint>(mesh->indices.size());

        m_vertexOffsets[mesh] = vertexBufferOffset;
        m_indexOffsets[mesh] = indexBufferOffset;

        vertices.insert(vertices.end(),mesh->vertices.begin(), mesh->vertices.end());
        indices.insert(indices.end(), mesh->indices.begin(), mesh->indices.end());

        vertexBufferOffset += vertexCount;
        indexBufferOffset += indexCount;
    }

    m_vertexBuffer = std::make_unique<Buffer<Vertex>>(vertices);
    m_indexBuffer = std::make_unique<Buffer<GLuint>>(indices);
}

MeshBuffer::~MeshBuffer()
{
}

void MeshBuffer::bindToVertexLayout(const VertexLayout& vertexLayout) const
{
    vertexLayout.bindVertexBuffer(0, m_vertexBuffer->handle(), 0, sizeof(Vertex));
    vertexLayout.bindElementBuffer(m_indexBuffer->handle());
}
