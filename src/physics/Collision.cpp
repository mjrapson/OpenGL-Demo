// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Collision.h"

#include "data/Box.h"
#include "data/Ray.h"
#include "data/Sphere.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace collision
{
bool intersects(const Ray& ray, const Box& box, float& distance)
{
    const auto inverseDirection = glm::vec3(
        ray.direction().x != 0.0f ? 1.0f / ray.direction().x : std::numeric_limits<float>::infinity(),
        ray.direction().y != 0.0f ? 1.0f / ray.direction().y : std::numeric_limits<float>::infinity(),
        ray.direction().z != 0.0f ? 1.0f / ray.direction().z : std::numeric_limits<float>::infinity());

    // Calculate distance along ray that the ray would theoretically intersect
    const auto t0 = (box.min() - ray.position()) * inverseDirection;
    const auto t1 = (box.max() - ray.position()) * inverseDirection;

    // Make sure the smallest (entry) is first  
    const auto tMin = glm::min(t0, t1);
    const auto tMax = glm::max(t0, t1);

    // Calculate the entry and exist as distances along the ray
    const auto entry = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    const auto exit = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

    // Validate that the entry and exit distances along the ray are valid and therefore indicate a collision
    if (entry > exit || exit < 0.0f)
    {
        return false;
    }

    distance = entry;

    return true;
}

bool intersects(const Sphere& sphere, const Box& box)
{
    // Sphere center coordinates
    const glm::vec3 sphereCenter = sphere.centerPoint();

    // Find the closest point on the box to the sphere center
    const auto closestPoint = glm::clamp(sphereCenter, box.min(), box.max());

    // Compute the squared distance from the sphere center to the closest point
    const auto distanceSquared = glm::length2(sphereCenter - closestPoint);

    // Calculate squared radius
    float radiusSquared = sphere.radius() * sphere.radius();

    // Intersection occurs if the squared distance is less than or equal to the squared radius
    return distanceSquared <= radiusSquared;
}
}
