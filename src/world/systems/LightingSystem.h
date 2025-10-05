/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class Renderer;
class World;

class LightingSystem
{
    public:
        LightingSystem(Renderer& renderer, World& world);

        void update();

    private:
        Renderer& m_renderer;
        World& m_world;
};
