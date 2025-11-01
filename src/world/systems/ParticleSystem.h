/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class Renderer;
class World;

class ParticleSystem
{
    public:
        ParticleSystem(Renderer& renderer, World& world);

        void update(float deltaTime);

    private:
        Renderer& m_renderer;
        World& m_world;
};
