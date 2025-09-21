// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <filesystem>
#include <memory>

class Texture;

std::unique_ptr<Texture> loadTexture(const std::filesystem::path& path);
