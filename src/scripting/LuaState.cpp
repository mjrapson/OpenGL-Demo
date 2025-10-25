/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "LuaState.h"

sol::state& LuaState::lua()
{
    return m_luaState;
}
