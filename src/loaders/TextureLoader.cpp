// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "TextureLoader.h"

#include "data/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/gl.h>

#include <memory>
#include <vector>

std::unique_ptr<Texture> loadTexture(const std::filesystem::path& path)
{
    int width;
    int height;
    int components;

    stbi_set_flip_vertically_on_load(true);
    const auto data = stbi_load(path.c_str(), &width, &height, &components, 4);

    auto texture = std::make_unique<Texture2D>(GL_RGBA8, width, height);

    texture->writeImageData(width, height, data);

    stbi_image_free(data);

    return texture;
}

std::unique_ptr<TextureCubeMap> loadCubemapTexture(const CubemapFileInfo& cubemapFileInfo)
{
    int width;
    int height;
    int components;

    stbi_set_flip_vertically_on_load(true);
    const auto dataPx = stbi_load(cubemapFileInfo.px.c_str(), &width, &height, &components, 4);
    const auto dataNx = stbi_load(cubemapFileInfo.nx.c_str(), &width, &height, &components, 4);
    const auto dataPy = stbi_load(cubemapFileInfo.py.c_str(), &width, &height, &components, 4);
    const auto dataNy = stbi_load(cubemapFileInfo.ny.c_str(), &width, &height, &components, 4);
    const auto dataPz = stbi_load(cubemapFileInfo.pz.c_str(), &width, &height, &components, 4);
    const auto dataNz = stbi_load(cubemapFileInfo.nz.c_str(), &width, &height, &components, 4);

    auto texture = std::make_unique<TextureCubeMap>(GL_RGBA8, width, height);

    texture->writeImageData(width, height, 0, dataPx);
    texture->writeImageData(width, height, 1, dataNx);
    texture->writeImageData(width, height, 2, dataPy);
    texture->writeImageData(width, height, 3, dataNy);
    texture->writeImageData(width, height, 4, dataPz);
    texture->writeImageData(width, height, 5, dataNz);

    stbi_image_free(dataPx);
    stbi_image_free(dataNx);
    stbi_image_free(dataPy);
    stbi_image_free(dataNy);
    stbi_image_free(dataPz);
    stbi_image_free(dataNz);

    return texture;
}
