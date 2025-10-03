/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "InputHandler.h"

#include <GLFW/glfw3.h>

void InputHandler::setKeyPressed(int key)
{
    m_keyState[key] = true;
}

void InputHandler::setKeyReleased(int key)
{
    m_keyState[key] = false;
}

bool InputHandler::isKeyPressed(int key) const
{
    if(!m_keyState.contains(key))
    {
        m_keyState[key] = false;
    }

    return m_keyState.at(key);
}
