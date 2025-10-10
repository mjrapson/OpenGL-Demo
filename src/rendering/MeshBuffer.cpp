/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "MeshBuffer.h"

#include "data/Mesh.h"
#include "rendering/VertexLayout.h"

MeshBuffer::MeshBuffer(const Container<Mesh>& meshes)
{
    auto vertexBufferOffset = GLuint{0};
    auto indexBufferOffset = GLuint{0};

    auto vertices = std::vector<Vertex>{};
    auto indices = std::vector<GLuint>{};

    for (const auto& mesh : meshes.view())
    {
        const auto meshVertices = mesh->vertices();
        const auto meshIndices = mesh->indices();

        const auto vertexCount = static_cast<GLuint>(meshVertices.size());
        const auto indexCount = static_cast<GLuint>(meshIndices.size());

        m_vertexOffsets[mesh] = vertexBufferOffset;
        m_indexOffsets[mesh] = indexBufferOffset;

        vertices.insert(vertices.end(),meshVertices.begin(), meshVertices.end());
        indices.insert(indices.end(), meshIndices.begin(), meshIndices.end());

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
