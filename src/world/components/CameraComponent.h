/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/Camera.h"

struct CameraComponent
{
    Camera camera;
    bool active{true};
};
