/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "LuaState.h"

LuaState::LuaState()
{
    m_luaState.open_libraries(sol::lib::base, sol::lib::math);
}

sol::state& LuaState::lua()
{
    return m_luaState;
}
