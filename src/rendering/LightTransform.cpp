/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "LightTransform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

glm::mat4 getLightSpaceMatrix(const glm::vec3& lightDirection)
{
    // Directional lights have no position, so make a pretend one along its direction vector
    const auto normalisedDirection = glm::normalize(lightDirection);
    const auto lightPos = -normalisedDirection * 50.0f;

    // Create a projection and view from the pretend light point of view
    const auto lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 200.0f);
    const auto lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));

    return lightProjection * lightView;
}

std::array<glm::mat4, 6> getPointLightShadowTransforms(const glm::vec3& position, float farPlane)
{
    const auto shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);

    return std::array<glm::mat4, 6>{
        shadowProj * glm::lookAt(position, position + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),  // +X (Right)
        shadowProj * glm::lookAt(position, position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)), // -X (Left)
        shadowProj * glm::lookAt(position, position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),   // +Y (Up)
        shadowProj * glm::lookAt(position, position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)), // -Y (Down)
        shadowProj * glm::lookAt(position, position + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),  // +Z (Forward)
        shadowProj * glm::lookAt(position, position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))  // -Z (Backward)
    };
}
