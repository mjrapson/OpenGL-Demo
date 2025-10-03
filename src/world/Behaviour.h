/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "world/Entity.h"

class InputHandler;
class World;

class Behaviour
{
    public:
        virtual ~Behaviour() {}

        virtual void update(Entity entity, World& world, float deltaTime, const InputHandler& inputHandler) const = 0;
};
