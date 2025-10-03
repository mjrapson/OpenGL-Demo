/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "world/Behaviour.h"

class CameraMoveBehaviour : public Behaviour
{
    public:
        void update(Entity entity, World& world, float deltaTime, const InputHandler& inputHandler) const override; 
};
