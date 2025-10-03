/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/MeshInstance.h"

#include <vector>

struct MeshRendererComponent
{
    std::vector<MeshInstance> meshInstances;

    MeshRendererComponent& addMeshInstance(Material* material, Mesh* mesh)
    {
        auto instance = MeshInstance{};
        instance.material = material;
        instance.mesh = mesh;
        meshInstances.push_back(instance);

        return *this;
    }

    MeshRendererComponent& addMeshInstances(const std::vector<MeshInstance>& instances)
    {
        meshInstances.insert(meshInstances.end(), instances.begin(), instances.end());
        return *this;
    }
};
