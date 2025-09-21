// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class Scene3DModel;

class SceneSelection
{
    public:
        void setHoveredObject(Scene3DModel* object);
        void setSelectedObject(Scene3DModel* object);

        void clearHover();
        void clearSelection();

        Scene3DModel* hoveredObject() const;
        Scene3DModel* selectedObject() const;

    private:
        Scene3DModel* m_hovereddObject{nullptr};
        Scene3DModel* m_selectedObject{nullptr};
};
