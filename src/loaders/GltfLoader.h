// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/MeshInstance.h"

#include <filesystem>
#include <vector>

class AssetDatabase;

std::vector<MeshInstance> loadGLTFModel(const std::filesystem::path& path, AssetDatabase& assetDb);
