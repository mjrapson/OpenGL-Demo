/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class Camera;
class Renderer;
class World;

class RenderSystem
{
    public:
        RenderSystem(const Renderer& renderer, World& world);

        void draw(const Camera& camera) const;

    private:
        const Renderer& m_renderer;
        World& m_world;
};
