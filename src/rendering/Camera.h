// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

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
    float pitch{0.0f}; // horizontal
    float yaw{0.0f};   // vertical
    float roll{0.0f};
    std::optional<Skybox*> skybox;
};
