// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Sphere.h"

Sphere::Sphere(const glm::vec3& center, float radius)
    : m_center(center),
      m_radius(radius)
{
}

const glm::vec3& Sphere::centerPoint() const
{
    return m_center;
}

float Sphere::radius() const
{
    return m_radius;
}