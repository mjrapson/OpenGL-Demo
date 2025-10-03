/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

struct TransformComponent
{
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    TransformComponent& setPosition(float x, float y, float z)
    {
        position = glm::vec3{x, y, z};
        return *this;
    }

    TransformComponent& setRotation(float x, float y, float z)
    {
        rotation = glm::vec3{x, y, z};
        return *this;
    }

    TransformComponent& setScale(float x, float y, float z)
    {
        scale = glm::vec3{x, y, z};
        return *this;
    }
};
