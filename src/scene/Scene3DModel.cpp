// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Scene3DModel.h"

#include "data/Mesh.h"

void Scene3DModel::addMeshInstance(const MeshInstance& instance)
{
    m_meshInstances.push_back(instance);
    updateBoundingBox();
}

const std::vector<MeshInstance>& Scene3DModel::meshInstances() const
{
    return m_meshInstances;
}

Box Scene3DModel::boundingBox() const
{
    auto worldBBox = m_boundingBox;
    worldBBox.transform(transformMatrix());
    return worldBBox;
}

void Scene3DModel::updateBoundingBox()
{
    if(m_meshInstances.empty())
    {
        m_boundingBox = Box{};
        return;
    }

    m_boundingBox = m_meshInstances.at(0).mesh->boundingBox();

    for(int i = 1; i < m_meshInstances.size(); ++i)
    {
        m_boundingBox.expandToFit(m_meshInstances.at(i).mesh->boundingBox());
    }
}
