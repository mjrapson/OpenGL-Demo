// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <optional>

class Skybox;

struct Camera
{
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float fieldOfView{45.0f};
    float nearPlane{0.1f};
    float farPlane{1000.0f};
    float aspectRatio{1.0f};
    float pitch{0.0f}; // horizontal
    float yaw{0.0f};   // vertical
    float roll{0.0f};
    std::optional<Skybox*> skybox;

    const glm::mat4 view() const
    {
        return glm::lookAt(position, position + front, up);
    }

    const glm::mat4 projection() const
    {
        return glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
    }
};
