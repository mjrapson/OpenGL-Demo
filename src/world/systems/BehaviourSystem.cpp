/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "BehaviourSystem.h"

#include "world/Behaviour.h"
#include "world/World.h"

BehaviourSystem::BehaviourSystem(const InputHandler& inputHandler, World& world)
    : m_inputHandler{inputHandler}
    , m_world{world}
{
}

void BehaviourSystem::init()
{
    for(auto& [entity, behaviourComponent] : m_world.getAllComponents<BehaviourComponent>())
    {
        for(const auto& script : behaviourComponent.behaviours)
        {
            script->init(entity, m_world);
        }
    }
}

void BehaviourSystem::update(float deltaTime)
{
    for(auto& [entity, behaviourComponent] : m_world.getAllComponents<BehaviourComponent>())
    {
        for(const auto& script : behaviourComponent.behaviours)
        {
            script->update(entity, m_world, deltaTime, m_inputHandler);
        }
    }
}
