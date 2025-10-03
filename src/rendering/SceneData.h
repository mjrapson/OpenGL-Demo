/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/DirectionalLight.h"
#include "data/PointLight.h"

#include <vector>

class Camera;

struct SceneData
{
    const Camera* camera;
    DirectionalLight directionalLight;
    std::vector<PointLight> pointLights;
};
