// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "core/Vertex.h"

#include <vector>

#include <glad/gl.h>

class Material;

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    Material* material;
};
