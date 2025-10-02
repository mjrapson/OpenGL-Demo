// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 direction{0.0f, 0.0f, 0.0f};
    glm::vec3 color{0.0f, 0.0f, 0.0f};
};
