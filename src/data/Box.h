// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "core/Vertex.h"

#include <glm/glm.hpp>

#include <vector>

class Box
{
    public:
        Box();
        Box(const glm::vec3& min, const glm::vec3& max);

        const glm::vec3& min() const;
        const glm::vec3& max() const;

        glm::vec3 center() const;

        void expandToFit(const Box& box);
        void expandToFit(const glm::vec3& point);

        void transform(const glm::mat4& matrix);

        bool contains(const Box& box) const;
        bool intersects(const Box& box) const;

        std::vector<glm::vec3> vertices() const;

        static Box enclose(const std::vector<Vertex>& vertices);

    private:
        glm::vec3 m_min;
        glm::vec3 m_max;
};
