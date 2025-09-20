// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

struct Material;
class Mesh;
class Texture;

struct Container
{
    std::unordered_map<std::string, std::unique_ptr<Material>> materials;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};