/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "CameraComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

const Camera& CameraComponent::camera() const
{
    return m_camera;
}

const glm::vec3& CameraComponent::position() const
{
    return m_camera.position;
}

const glm::vec3& CameraComponent::front() const
{
    return m_camera.front;
}

const glm::vec3& CameraComponent::up() const
{
    return m_camera.up;
}

float CameraComponent::yaw() const
{
    return m_camera.yaw;
}

float CameraComponent::roll() const
{
    return m_camera.roll;
}

float CameraComponent::pitch() const
{
    return m_camera.pitch;
}

bool CameraComponent::active() const
{
    return m_active;
}

void CameraComponent::setPosition(const glm::vec3& position)
{
    m_camera.position = position;
}

void CameraComponent::setFront(const glm::vec3& front)
{
    m_camera.front = front;
}

void CameraComponent::setUp(const glm::vec3& up)
{
    m_camera.up = up;
}

void CameraComponent::setYaw(float yaw)
{
    m_camera.yaw = yaw;
    normalize();
}

void CameraComponent::setRoll(float roll)
{
    m_camera.roll = roll;
}

void CameraComponent::setPitch(float pitch)
{
    m_camera.pitch = pitch;
    normalize();
}

void CameraComponent::setActive(bool active)
{
    m_active = active;
}

void CameraComponent::setSkybox(Skybox* skybox)
{
    m_camera.skybox = skybox;
}

void CameraComponent::normalize()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_camera.yaw)) * cos(glm::radians(m_camera.pitch));
    front.y = sin(glm::radians(m_camera.pitch));
    front.z = sin(glm::radians(m_camera.yaw)) * cos(glm::radians(m_camera.pitch));

    m_camera.front = glm::normalize(front);
}
