/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#pragma once

class LuaState
{
    public:
        sol::state& lua();

    private:
        sol::state m_luaState;
};
