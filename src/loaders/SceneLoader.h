// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <filesystem>

class AssetDatabase;
class LuaState;
class World;

bool loadScene(const std::filesystem::path& path, AssetDatabase& assetDb, World& world, LuaState& lua);
