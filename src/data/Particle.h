/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

struct Particle
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 velocity;
    float age;
    float lifespan;
};
