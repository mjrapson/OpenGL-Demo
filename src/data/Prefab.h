/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/Box.h"

#include "data/Material.h"
#include "data/Mesh.h"
#include "data/Texture.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// class Material;
// class Mesh;
// class Texture;

class Prefab
{
    public:
        void addMaterial(const std::string& name, std::unique_ptr<Material> material);
        void addMesh(std::unique_ptr<Mesh> mesh);
        void addTexture(const std::string& name, std::unique_ptr<Texture> texture);

        Material* getMaterial(const std::string& name) const;
        Texture* getTexture(const std::string& name) const;

        const std::vector<std::unique_ptr<Mesh>>& meshes() const;
    
        const Box& boundingBox() const;

    private:
        std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
        std::vector<std::unique_ptr<Mesh>> m_meshes;
        std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

        Box m_boundingBox{};
};
