// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <filesystem>
#include <memory>

class Texture;
class TextureCubeMap;

std::unique_ptr<Texture> loadTexture(const std::filesystem::path& path);

struct CubemapFileInfo 
{
    std::filesystem::path px;
    std::filesystem::path py;
    std::filesystem::path pz;
    std::filesystem::path nx;
    std::filesystem::path ny;
    std::filesystem::path nz;
};
std::unique_ptr<TextureCubeMap> loadCubemapTexture(const CubemapFileInfo& cubemapFileInfo);
