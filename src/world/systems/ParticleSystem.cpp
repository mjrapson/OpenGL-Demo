/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "ParticleSystem.h"

#include "data/Particle.h"
#include "rendering/Renderer.h"
#include "world/World.h"

ParticleSystem::ParticleSystem(Renderer& renderer, World& world)
    : m_renderer{renderer}
    , m_world{world}
{
}

void ParticleSystem::update(float deltaTime)
{
}
