/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "BehaviourSystem.h"

#include "world/Behaviour.h"

#include "world/World.h"

BehaviourSystem::BehaviourSystem(World& world)
    : m_world{world}
{
}

void BehaviourSystem::update(float deltaTime) const
{
    for(auto& [entity, behaviourComponent] : m_world.getAllComponents<BehaviourComponent>())
    {
        for(const auto& script : behaviourComponent.behaviours)
        {
            script->update(m_world, deltaTime);
        }
    }
}
