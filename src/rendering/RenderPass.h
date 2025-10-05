/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/DirectionalLight.h"
#include "data/PointLight.h"
#include "rendering/DrawCommand.h"

#include <vector>

class MeshBuffer;

struct Camera;

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
                             const Camera& camera,
                             const DirectionalLight& directionalLight,
                             const std::vector<PointLight>& pointLights,
                             const MeshBuffer& buffer) = 0;
};
