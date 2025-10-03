/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class World;

class Behaviour
{
    public:
        virtual ~Behaviour() {}

        virtual void update(World& world, float deltaTime) const = 0;
};
