/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/Texture.h"

#include <memory>

struct Skybox
{
    std::unique_ptr<TextureCubeMap> cubemapTexture;
};
