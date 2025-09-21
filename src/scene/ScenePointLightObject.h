// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "Scene3DTransformObject.h"

#include "data/PointLight.h"
#include "data/Sphere.h"

#include <glm/glm.hpp>

class ScenePointLightObject : public Scene3DTransformObject
{
    public:
        Sphere boundingSphere() const;
        PointLight toLightData() const;

        void setRadius(float radius);
        void setColor(const glm::vec3& color);

    private:
        float m_radius{0.0f};
        glm::vec3 m_color{0.0f, 0.0f, 0.0f};
};