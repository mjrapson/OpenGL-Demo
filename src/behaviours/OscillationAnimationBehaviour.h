/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "world/Behaviour.h"

class OscillationAnimationBehaviour : public Behaviour
{
    public:
        void init(Entity entity, World& world) override;

        void update(Entity entity, World& world, float deltaTime, const InputHandler& inputHandler) override;

    private:
        float m_startY{0.0f};
        float m_elapsedTime{0.0f};
};
