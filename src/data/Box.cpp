// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Box.h"

Box::Box()
    : m_min{glm::vec3{0.0f}}
    , m_max{glm::vec3{0.0f}}
{
}

Box::Box(const glm::vec3& min, const glm::vec3& max)
    : m_min{min}
    , m_max{max}
{
}

const glm::vec3& Box::min() const
{
    return m_min;
}

const glm::vec3& Box::max() const
{
    return m_max;
}

glm::vec3 Box::center() const
{
    return (m_min + m_max) * 0.5f;
}

void Box::expandToFit(const Box& box)
{
    m_min = glm::min(m_min, box.min());
    m_max = glm::max(m_max, box.max());
}

void Box::expandToFit(const glm::vec3& point)
{
    m_min = glm::min(m_min, point);
    m_max = glm::max(m_max, point);
}

void Box::transform(const glm::mat4& matrix)
{
    auto newMin = glm::vec3{std::numeric_limits<float>::max()};
    auto newMax = glm::vec3{-std::numeric_limits<float>::max()};

    for (const auto& corner :
         {glm::vec4(m_min.x, m_min.y, m_min.z, 1.0f),
          glm::vec4(m_min.x, m_min.y, m_max.z, 1.0f),
          glm::vec4(m_min.x, m_max.y, m_min.z, 1.0f),
          glm::vec4(m_min.x, m_max.y, m_max.z, 1.0f),
          glm::vec4(m_max.x, m_min.y, m_min.z, 1.0f),
          glm::vec4(m_max.x, m_min.y, m_max.z, 1.0f),
          glm::vec4(m_max.x, m_max.y, m_min.z, 1.0f),
          glm::vec4(m_max.x, m_max.y, m_max.z, 1.0f)})
    {
        const auto transformedCorner = matrix * corner;
        const auto transformedPoint = glm::vec3(transformedCorner);

        newMin = glm::min(newMin, transformedPoint);
        newMax = glm::max(newMax, transformedPoint);
    }

    m_min = newMin;
    m_max = newMax;
}

bool Box::contains(const Box& box) const
{
    return glm::all(glm::greaterThanEqual(box.min(), m_min)) && glm::all(glm::lessThanEqual(box.max(), m_max));
}

bool Box::intersects(const Box& box) const
{
    return glm::all(glm::lessThanEqual(box.min(), m_max)) && glm::all(glm::greaterThanEqual(box.max(), m_min));
}

std::vector<glm::vec3> Box::vertices() const
{
    // cache this...
    return std::vector<glm::vec3>{
        glm::vec3(m_min.x, m_min.y, m_min.z),
        glm::vec3(m_min.x, m_min.y, m_max.z),
        glm::vec3(m_min.x, m_max.y, m_min.z),
        glm::vec3(m_min.x, m_max.y, m_max.z),
        glm::vec3(m_max.x, m_min.y, m_min.z),
        glm::vec3(m_max.x, m_min.y, m_max.z),
        glm::vec3(m_max.x, m_max.y, m_min.z),
        glm::vec3(m_max.x, m_max.y, m_max.z)};
}

Box Box::enclose(const std::vector<Vertex>& vertices)
{
    if(vertices.empty() || vertices.size() < 2)
    {
        return Box{};
    }

    auto box = Box{vertices.at(0).position, vertices.at(1).position};
    for(auto i = 2; i < vertices.size(); ++i)
    {
        box.expandToFit(vertices.at(i).position);
    }

    return box;
}
