/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "world/Behaviour.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <memory>

class LuaScript;

class LuaBehaviour : public Behaviour
{
    public:
        LuaBehaviour(std::unique_ptr<LuaScript> script);

        void init(Entity entity, World& world) override;

        void update(Entity entity, World& world, float deltaTime, const InputHandler& inputHandler) override;

    private:
        std::unique_ptr<LuaScript> m_script;
};
