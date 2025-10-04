/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class InputHandler;
class World;

class BehaviourSystem
{
    public:
        BehaviourSystem(const InputHandler& inputHandler, World& world);

        void init();
        void update(float deltaTime);

    private:
        const InputHandler& m_inputHandler;
        World& m_world;
};
