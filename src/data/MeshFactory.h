// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <memory>
#include <string>

class Mesh;

class MeshFactory
{
public:
    static std::unique_ptr<Mesh> createCubePrimitive(const std::string& name);
    static std::unique_ptr<Mesh> createSpherePrimitive(const std::string& name);
    static std::unique_ptr<Mesh> createPlanePrimitive(const std::string& name);
};
