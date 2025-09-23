// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <filesystem>
#include <memory>

class Container;
class Scene3DModel;

std::unique_ptr<Scene3DModel> loadGLTFModel(const std::filesystem::path& path, Container& container);
