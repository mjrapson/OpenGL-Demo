// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "Scene3DTransformObject.h"

#include "data/Box.h"
#include "data/MeshInstance.h"

#include <vector>

class Scene3DModel : public Scene3DTransformObject
{
    public:
        void addMeshInstance(const MeshInstance& instance);

        const std::vector<MeshInstance>& meshInstances() const;

        Box boundingBox() const;

    private:
        void updateBoundingBox();

    private:
        std::vector<MeshInstance> m_meshInstances;
        Box m_boundingBox;
};
