/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/Prefab.h"
#include "data/Skybox.h"

class AssetDatabase
{
    public:
        ~AssetDatabase();

        void addPrefab(const std::string& name, std::unique_ptr<Prefab> prefab);
        void addSkybox(const std::string& name, std::unique_ptr<Skybox> skybox);

        const std::unordered_map<std::string, std::unique_ptr<Prefab>>& prefabs() const;
        const std::unordered_map<std::string, std::unique_ptr<Skybox>>& skyboxes() const;

        void clearAll();

    private:
        std::unordered_map<std::string, std::unique_ptr<Prefab>> m_prefabs;
        std::unordered_map<std::string, std::unique_ptr<Skybox>> m_skyboxes;
};
