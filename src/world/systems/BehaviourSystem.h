/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class World;

class BehaviourSystem
{
    public:
        explicit BehaviourSystem(World& world);

        void update(float deltaTime) const;

    private:
        World& m_world;
};
