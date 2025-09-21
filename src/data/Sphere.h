// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

class Sphere
{
    public:
        Sphere(const glm::vec3& center, float radius);

        const glm::vec3& centerPoint() const;
        float radius() const;

    private:
        glm::vec3 m_center{0.0f, 0.0f, 0.0f};
        float m_radius{0.0f};
};