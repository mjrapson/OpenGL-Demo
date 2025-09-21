// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "SceneSelection.h"

#include "Scene3DModel.h"

void SceneSelection::setHoveredObject(Scene3DModel* object)
{
    clearHover();

    m_hovereddObject = object;
    m_hovereddObject->setHovered(true);
}

void SceneSelection::setSelectedObject(Scene3DModel* object)
{
    m_selectedObject = object;
}

void SceneSelection::clearHover()
{
    if (m_hovereddObject)
    {
        m_hovereddObject->setHovered(false);
        m_hovereddObject = nullptr;
    }
}

void SceneSelection::clearSelection()
{
    m_selectedObject = nullptr;
}

Scene3DModel* SceneSelection::hoveredObject() const
{
    return m_hovereddObject;
}

Scene3DModel* SceneSelection::selectedObject() const
{
    return m_selectedObject;
}