/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "AssetDatabase.h"

AssetDatabase::~AssetDatabase()
{
    clearAll();
}

void AssetDatabase::addPrefab(const std::string& name, std::unique_ptr<Prefab> prefab)
{
    m_prefabs[name] = std::move(prefab);
}
void AssetDatabase::addSkybox(const std::string& name, std::unique_ptr<Skybox> skybox)
{
    m_skyboxes[name] = std::move(skybox);
}

const std::unordered_map<std::string, std::unique_ptr<Prefab>>& AssetDatabase::prefabs() const
{
    return m_prefabs;
}

const std::unordered_map<std::string, std::unique_ptr<Skybox>>& AssetDatabase::skyboxes() const
{
    return m_skyboxes;
}

void AssetDatabase::clearAll()
{
    m_prefabs.clear();
    m_skyboxes.clear();
}
