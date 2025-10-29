/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "ScriptLoader.h"

#include "scripting/LuaScript.h"
#include "scripting/LuaState.h"

#include <iostream>

std::unique_ptr<LuaScript> loadScript(const std::filesystem::path& path, LuaState& lua)
{
    auto chunk = lua.lua().load_file(path.string());
    if(!chunk.valid())
    {
        sol::error err = chunk;
        std::cerr << "Failed to load script: " << path.string() << ", " << err.what() << "\n";
        return nullptr;
    }

    auto result = chunk();
    if(!result.valid())
    {
        sol::error err = result;
        std::cerr << "Failed to read lua: " << path.string() << ", " << err.what() << "\n";
        return nullptr;
    }

    if(result.get_type() != sol::type::table)
    {
        std::cerr << "No table loaded: " << path.string() << "\n";
        return nullptr;
    }

    sol::table table = result;
    return std::make_unique<LuaScript>(table);
}
