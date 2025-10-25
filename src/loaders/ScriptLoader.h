/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <filesystem>
#include <memory>

class LuaScript;
class LuaState;

std::unique_ptr<LuaScript> loadScript(const std::filesystem::path& path, LuaState& lua);
