// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class Material;
class Mesh;

struct MeshInstance
{
    Material* material;
    Mesh* mesh;
};
