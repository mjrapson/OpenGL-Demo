// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

class Ray
{
    public:
        Ray(const glm::vec3& start, const glm::vec3& direction)
            : m_position{start},
              m_direction(direction)
        {
        }

        inline const glm::vec3& position() const
        {
            return m_position;
        }

        inline const glm::vec3& direction() const
        {
            return m_direction;
        }
    private:
        glm::vec3 m_position;
        glm::vec3 m_direction;
};
