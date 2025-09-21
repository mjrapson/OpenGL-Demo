// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "SceneObject.h"

#include "data/Box.h"

#include <glm/glm.hpp>

class Scene3DTransformObject : public SceneObject
{
    public:
        const glm::vec3& position() const;
        const glm::vec3& rotation() const;
        const glm::vec3& scale() const;
        bool hovered() const;
        glm::mat4 transformMatrix() const;

        void setPosition(const glm::vec3& position);
        void setRotation(const glm::vec3& rotation);
        void setScale(const glm::vec3& scale);
        void setHovered(bool isHovered);

    private:
        glm::vec3 m_position{0.0f, 0.0f, 0.0f};
        glm::vec3 m_rotation{0.0f, 0.0f, 0.0f};
        glm::vec3 m_scale{1.0f, 1.0f, 1.0f};
        bool m_isHovered{false};
};
