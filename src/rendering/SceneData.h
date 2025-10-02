/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <vector>

class Camera;
struct DirectionalLight;
struct PointLight;

struct SceneData
{
    const Camera* camera;
    const DirectionalLight* directionalLight;
    std::vector<PointLight> pointLights;
};
