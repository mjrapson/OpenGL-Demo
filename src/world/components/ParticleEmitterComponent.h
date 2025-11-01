/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/Particle.h"

#include <glm/glm.hpp>

#include <vector>

struct ParticleEmitterComponent
{
    int maxParticles;
    float particleLifespan;
    float emissionRate;
    bool enabled;
    bool looping;
    glm::vec3 position;
    glm::vec3 initialVelocity;
    glm::vec3 startColor;
    glm::vec3 endColor;
    std::vector<Particle> particles;
};
