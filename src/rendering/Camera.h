// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glm/glm.hpp>

class Camera
{
    public:
        Camera();
        
        const glm::mat4 projection() const;
        const glm::mat4 view() const;

        const glm::vec3& position() const;
        const glm::vec3& front() const;
        const glm::vec3& up() const;
        float yaw() const;
        float roll() const;
        float pitch() const;
        
        void setPosition(const glm::vec3& position);
        void setFront(const glm::vec3& front);
        void setUp(const glm::vec3& up);
        void setYaw(float yaw);
        void setRoll(float roll);
        void setPitch(float pitch);
        void setAspectRatio(float val);

    private:
        void normalize();

    private:
        glm::vec3 m_position{0.0f, 0.0f, 0.0f};
        glm::vec3 m_front{0.0f, 0.0f, -1.0f};
        glm::vec3 m_up{0.0f, 1.0f, 0.0f};

        float m_fieldOfView{45.0f};
        float m_nearPlane{0.1f};
        float m_farPlane{1000.0f};
        float m_aspectRatio{1.0f};

        float m_pitch{0.0f}; // horizontal
        float m_yaw{0.0f};   // vertical
        float m_roll{0.0f};

        float m_angleAroundTarget{0.0f};
        float m_distanceFromTarget{0.0f};
};