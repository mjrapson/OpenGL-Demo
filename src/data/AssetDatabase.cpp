/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "AssetDatabase.h"

#include "data/Material.h"
#include "data/Mesh.h"
#include "data/Texture.h"

AssetDatabase::~AssetDatabase()
{
    clearAll();
}

Container<Material>& AssetDatabase::materialContainer()
{
    return m_materialContainer;
}

const Container<Material>& AssetDatabase::materialContainer() const
{
    return m_materialContainer;
}

Container<Mesh>& AssetDatabase::meshContainer()
{
    return m_meshContainer;
}

const Container<Mesh>& AssetDatabase::meshContainer() const
{
    return m_meshContainer;
}

Container<Texture>& AssetDatabase::textureContainer()
{
    return m_textureContainer;
}

const Container<Texture>& AssetDatabase::textureContainer() const
{
    return m_textureContainer;
}

void AssetDatabase::clearAll()
{
    m_materialContainer.clear();
    m_meshContainer.clear();
    m_textureContainer.clear();
}
