/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "LuaBehaviour.h"

#include "input/InputHandler.h"
#include "scripting/LuaScript.h"
#include "world/World.h"

LuaBehaviour::LuaBehaviour(std::unique_ptr<LuaScript> script)
    : m_script{std::move(script)}
{
}

void LuaBehaviour::init(Entity entity, World& world)
{
    if(!m_script)
    {
        return;
    }

    auto initFunction = m_script->table()["init"];
    if(!initFunction.valid())
    {
        return;
    }

    auto result = initFunction(m_script->table(), entity, world);
    if(!result.valid())
    {
        sol::error err = result;
        std::cerr << err.what() << "\n";
    }
}

void LuaBehaviour::update(Entity entity, World& world, float deltaTime, const InputHandler& inputHandler) 
{
    if(!m_script)
    {
        return;
    }

    auto updateFunction = m_script->table()["update"];
    if(!updateFunction.valid())
    {
        return;
    }

    auto result = updateFunction(m_script->table(), entity, world, deltaTime, inputHandler);
    if(!result.valid())
    {
        sol::error err = result;
        std::cerr << err.what() << "\n";
    }
}
