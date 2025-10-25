/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

class LuaState
{
    public:
        LuaState();

        sol::state& lua();

    private:
        sol::state m_luaState;
};
