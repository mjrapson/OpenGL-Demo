/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

class LuaScript
{
    public:
        LuaScript(sol::table& table);
        ~LuaScript();

        LuaScript(const LuaScript& other) = delete;
        LuaScript(LuaScript&& other) = delete;

        LuaScript& operator=(const LuaScript& other) = delete;
        LuaScript& operator=(LuaScript&& other) = delete;

        sol::table table() const;

    private:
        sol::table m_table;
};
