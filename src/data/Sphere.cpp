// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Sphere.h"

Sphere::Sphere(float x, float y, float z, float radius)
    : m_center(glm::vec3{x, y, z}),
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