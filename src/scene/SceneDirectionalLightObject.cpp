// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "SceneDirectionalLightObject.h"

DirectionalLight SceneDirectionalLightObject::toLightData() const
{
    return DirectionalLight{
        .direction = m_direction, 
        .color = m_color,
    };
}

void SceneDirectionalLightObject::setDirection(const glm::vec3& direction)
{
    m_direction = direction;
}

void SceneDirectionalLightObject::setColor(const glm::vec3& color)
{
    m_color = color;
}