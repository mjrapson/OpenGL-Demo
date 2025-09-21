// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "SceneObject.h"

SceneObject::~SceneObject() = default;

const std::string& SceneObject::objectName() const
{
    return m_objectName;
}

void SceneObject::setObjectName(const std::string& name)
{
    m_objectName = name;
}
