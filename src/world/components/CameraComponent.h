/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "rendering/Camera.h"

#include <glm/glm.hpp>

class Skybox;

class CameraComponent
{
    public:
        const Camera& camera() const;
        const glm::vec3& position() const;
        const glm::vec3& front() const;
        const glm::vec3& up() const;
        float yaw() const;
        float roll() const;
        float pitch() const;
        bool active() const;
        
        void setPosition(const glm::vec3& position);
        void setFront(const glm::vec3& front);
        void setUp(const glm::vec3& up);
        void setYaw(float yaw);
        void setRoll(float roll);
        void setPitch(float pitch);
        void setAspectRatio(float val);
        void setActive(bool active);
        void setSkybox(Skybox* skybox);

    private:
        void normalize();
        
    private:
        Camera m_camera;
        bool m_active{true};
};
