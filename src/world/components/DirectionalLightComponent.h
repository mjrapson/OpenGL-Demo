/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

struct DirectionalLightComponent
{
    glm::vec3 direction{0.0f};
    glm::vec3 color{1.0f};

    DirectionalLightComponent& setDirection(float x, float y, float z)
    {
        direction = glm::vec3{x, y, z};
        return *this;
    }

    DirectionalLightComponent& setColor(float r, float g, float b)
    {
        color = glm::vec3{r, g, b};
        return *this;
    }
};
