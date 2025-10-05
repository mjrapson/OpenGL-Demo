/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "LightingSystem.h"

#include "data/PointLight.h"
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
        auto light = DirectionalLight{};
        light.color = lightComponent.color;
        light.direction = lightComponent.direction;
        m_renderer.setDirectionalLight(light);
    }

    for(auto& [entity, lightComponent] : m_world.getAllComponents<PointLightComponent>())
    {
        auto transformComponent = m_world.getComponent<TransformComponent>(entity);
        if(!transformComponent)
        {
            continue;
        }

        auto pointLight = PointLight{};
        pointLight.color = lightComponent.color;
        pointLight.radius = lightComponent.radius;
        pointLight.position = transformComponent->position;
        m_renderer.addPointLight(pointLight);    
    }
}
