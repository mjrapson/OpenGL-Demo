// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

class Mesh;

struct DrawCommand
{
    Mesh* mesh;
    glm::mat4 transform;
};
