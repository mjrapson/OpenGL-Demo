// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

struct PointLight
{
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 color{0.0f, 0.0f, 0.0f};
    float radius = 7.5f;
};