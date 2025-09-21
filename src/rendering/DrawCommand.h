// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/MeshInstance.h"

#include <glm/glm.hpp>

struct DrawCommand
{
    MeshInstance instance;
    glm::mat4 transform;
};
