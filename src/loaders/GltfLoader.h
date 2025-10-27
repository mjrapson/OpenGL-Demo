// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <filesystem>
#include <memory>

class Prefab;

std::unique_ptr<Prefab> loadGLTFModel(const std::filesystem::path& path);
