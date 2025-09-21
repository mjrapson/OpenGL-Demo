// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "ScenePointLightObject.h"

Sphere ScenePointLightObject::boundingSphere() const
{
    return Sphere{position(), m_radius};
}

PointLight ScenePointLightObject::toLightData() const
{
    return PointLight{
        .position = position(), 
        .color = m_color, 
        .radius  = m_radius
    };
}

void ScenePointLightObject::setRadius(float radius)
{
    m_radius = radius;
}

void ScenePointLightObject::setColor(const glm::vec3& color)
{
    m_color = color;
}