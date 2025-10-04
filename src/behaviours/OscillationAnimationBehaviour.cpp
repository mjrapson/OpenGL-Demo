/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "OscillationAnimationBehaviour.h"

#include "world/World.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

void OscillationAnimationBehaviour::init(Entity entity, World& world)
{
    auto transformComponent = world.getComponent<TransformComponent>(entity);
    if(!transformComponent)
    {
        return;
    }

    m_startY = transformComponent->position.y;
}

void OscillationAnimationBehaviour::update(Entity entity, World& world, float deltaTime, const InputHandler& inputHandler) 
{
    auto transformComponent = world.getComponent<TransformComponent>(entity);
    if(!transformComponent)
    {
        return;
    }

    constexpr auto amplitude = 1.0f;
    constexpr auto frequency = 1.0f;

    // Update elapsed time
    m_elapsedTime += deltaTime;

    // Calculate sine offset
    const auto offset = amplitude * std::sin(glm::two_pi<float>() * frequency * m_elapsedTime);

    // Apply oscillation
    transformComponent->position.y = m_startY + offset;
}
