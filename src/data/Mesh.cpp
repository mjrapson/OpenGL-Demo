// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Mesh.h"

#include "core/Vertex.h"

Mesh::Mesh(std::unique_ptr<MeshData> data)
    : m_data{std::move(data)}
{
    calculateBoundingBox();
}

void Mesh::calculateBoundingBox()
{
    if (m_data->vertices.empty())
    {
        m_boundingBox = Box{};
    }

    m_boundingBox = Box{m_data->vertices.at(0).position, m_data->vertices.at(0).position};

    for (auto i = 1; i < m_data->vertices.size(); i++)
    {
        m_boundingBox.expandToFit(m_data->vertices.at(i).position);
    }
}

const std::vector<Vertex>& Mesh::vertices() const
{
    return m_data->vertices;
}

const std::vector<unsigned int>& Mesh::indices() const
{
    return m_data->indices;
}

const Box& Mesh::boundingBox() const
{
    return m_boundingBox;
}
