// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

#include <string>

struct Material
{
    std::string name;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    std::string diffuseTextureId;
};