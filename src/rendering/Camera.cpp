// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Camera.h"

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

Camera::Camera()
{
    normalize();
}

const glm::mat4 Camera::view() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

const glm::mat4 Camera::projection() const
{
    return glm::perspective(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
}

const glm::vec3& Camera::position() const
{
    return m_position;
}

const glm::vec3& Camera::front() const
{
    return m_front;
}

const glm::vec3& Camera::up() const
{
    return m_up;
}

float Camera::yaw() const
{
    return m_yaw;
}

float Camera::roll() const
{
    return m_roll;
}

float Camera::pitch() const
{
    return m_pitch;
}

void Camera::setPosition(const glm::vec3& position)
{
    m_position = position;
}

void Camera::setFront(const glm::vec3& front)
{
    m_front = front;
}

void Camera::setUp(const glm::vec3& up)
{
    m_up = up;
}

void Camera::setYaw(float yaw)
{
    m_yaw = yaw;
    normalize();
}

void Camera::setRoll(float roll)
{
    m_roll = roll;
}

void Camera::setPitch(float pitch)
{
    m_pitch = pitch;
    normalize();
}

void Camera::setAspectRatio(float val)
{
    m_aspectRatio = val;
}

void Camera::normalize()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
}
