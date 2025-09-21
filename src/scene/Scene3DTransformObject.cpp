// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Scene3DTransformObject.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

const glm::vec3& Scene3DTransformObject::position() const
{
    return m_position;
}

const glm::vec3&Scene3DTransformObject::rotation() const
{
    return m_rotation;
}

const glm::vec3& Scene3DTransformObject::scale() const
{
    return m_scale;
}

bool Scene3DTransformObject::hovered() const
{
    return m_isHovered;
}

glm::mat4 Scene3DTransformObject::transformMatrix() const
{
    auto transform = glm::mat4(1.0f);
    auto translationMatrix = glm::mat4(1.0f);
    auto rotationMatrix = glm::mat4(1.0f);
    auto scaleMatrix = glm::mat4(1.0f);

    // Compute scale * rotation * translation
    translationMatrix = glm::translate(m_position);

    rotationMatrix *= glm::rotate(glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix *= glm::rotate(glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix *= glm::rotate(glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    scaleMatrix *= glm::scale(glm::mat4(1.0f), m_scale);

    transform = translationMatrix * rotationMatrix * scaleMatrix;
    return transform;
}

void Scene3DTransformObject::setPosition(const glm::vec3& position)
{
    m_position = position;
}

void Scene3DTransformObject::setRotation(const glm::vec3& rotation)
{
    m_rotation = rotation;
}

void Scene3DTransformObject::setScale(const glm::vec3& scale)
{
    m_scale = scale;
}

void Scene3DTransformObject::setHovered(bool isHovered)
{
    m_isHovered = isHovered;
}
