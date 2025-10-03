/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class Renderer;
class World;

class RenderSystem
{
    public:
        RenderSystem(const Renderer& renderer, World& world);

        void draw() const;

    private:
        const Renderer& m_renderer;
        World& m_world;
};
