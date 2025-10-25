/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "LuaScript.h"

LuaScript::LuaScript(sol::table& table)
    : m_table{table}
{
}

LuaScript::~LuaScript() = default;

sol::table LuaScript::table() const
{
    return m_table;
}
