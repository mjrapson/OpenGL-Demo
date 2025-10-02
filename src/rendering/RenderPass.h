/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/DrawCommand.h"

#include <vector>

class MeshBuffer;
struct SceneData;

class RenderPass
{
    public:
        RenderPass(){};
        virtual ~RenderPass() {};

        RenderPass(const RenderPass& other) = delete;
        RenderPass(RenderPass&& other) = delete;

        RenderPass& operator=(const RenderPass& other) = delete;
        RenderPass& operator=(RenderPass&& other) = delete;

        virtual void execute(const std::vector<DrawCommand>& drawQueue, 
                             const SceneData& sceneData, 
                             const MeshBuffer& buffer) = 0;
};
