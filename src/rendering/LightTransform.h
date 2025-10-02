/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

#include <array>

glm::mat4 getLightSpaceMatrix(const glm::vec3& lightDirection);

std::array<glm::mat4, 6> getPointLightShadowTransforms(const glm::vec3& position, float farPlane);
