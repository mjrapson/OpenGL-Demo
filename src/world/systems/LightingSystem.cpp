/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "LightingSystem.h"

#include "rendering/Renderer.h"
#include "world/World.h"

LightingSystem::LightingSystem(Renderer& renderer, World& world)
    : m_renderer{renderer}
    , m_world{world}
{
}

void LightingSystem::update()
{
    const auto worldLightComponents = m_world.getAllComponents<DirectionalLightComponent>();
    for(auto& [entity, lightComponent] : worldLightComponents)
    {
        m_renderer.setDirectionalLight(lightComponent.light);
    }

    for(auto& [entity, lightComponent] : m_world.getAllComponents<PointLightComponent>())
    {
        m_renderer.addPointLight(lightComponent.light);    
    }
}
