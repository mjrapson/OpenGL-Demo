/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

struct PointLightComponent
{
    glm::vec3 color{1.0f};
    float radius{1.0f};

    PointLightComponent& setColor(float r, float g, float b)
    {
        color = glm::vec3{r, g, b};
        return *this;
    }

    PointLightComponent& setRadius(float r)
    {
        radius = r;
        return *this;
    }
};
