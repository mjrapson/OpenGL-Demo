// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "Scene3DTransformObject.h"

#include "data/DirectionalLight.h"
#include "data/Sphere.h"

#include <glm/glm.hpp>

class SceneDirectionalLightObject : public Scene3DTransformObject
{
    public:
        DirectionalLight toLightData() const;

        void setDirection(const glm::vec3& direction);
        void setColor(const glm::vec3& color);

    private:
        glm::vec3 m_direction{0.0f, 0.0f, 0.0f};
        glm::vec3 m_color{0.0f, 0.0f, 0.0f};
};