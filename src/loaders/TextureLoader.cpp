// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "TextureLoader.h"

#include "data/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/gl.h>

#include <memory>

std::unique_ptr<Texture> loadTexture(const std::filesystem::path& path)
{
    int width;
    int height;
    int components;

    stbi_set_flip_vertically_on_load(true);
    const auto data = stbi_load(path.c_str(), &width, &height, &components, 4);

    auto texture = std::make_unique<Texture2D>(GL_RGBA8, width, height);

    texture->writeImageData(width, height, data);
    texture->setMinFilter(GL_NEAREST);
    texture->setMagFilter(GL_NEAREST);
    texture->setWrapS(GL_REPEAT);
    texture->setWrapT(GL_REPEAT);

    stbi_image_free(data);

    return texture;
}
