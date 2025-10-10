// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <memory>

class Mesh;

class MeshFactory
{
public:
    static std::unique_ptr<Mesh> createCubePrimitive();
    static std::unique_ptr<Mesh> createSpherePrimitive();
    static std::unique_ptr<Mesh> createPlanePrimitive();
};
