/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "CameraMoveBehaviour.h"

#include "input/InputHandler.h"
#include "rendering/Camera.h"
#include "world/World.h"

#include <GLFW/glfw3.h>

void CameraMoveBehaviour::update(Entity entity, World& world, float deltaTime, const InputHandler& inputHandler) 
{
    auto cameraComponent = world.getComponent<CameraComponent>(entity);
    if(!cameraComponent)
    {
        return;
    }

    auto& camera = cameraComponent->camera;

    const auto speed = 10.0f;
    auto movement = glm::vec3(0.0f);

    if (inputHandler.isKeyPressed(GLFW_KEY_W)) movement += camera.front();
    if (inputHandler.isKeyPressed(GLFW_KEY_S)) movement -= camera.front();
    if (inputHandler.isKeyPressed(GLFW_KEY_A)) movement -= glm::normalize(glm::cross(camera.front(), camera.up()));
    if (inputHandler.isKeyPressed(GLFW_KEY_D)) movement += glm::normalize(glm::cross(camera.front(), camera.up()));
    if (inputHandler.isKeyPressed(GLFW_KEY_E)) movement += camera.up();
    if (inputHandler.isKeyPressed(GLFW_KEY_Q)) movement -= camera.up();

    if (glm::length(movement) > 0.0f)
    {
        movement = glm::normalize(movement) * speed * deltaTime;
        camera.setPosition(camera.position() + movement);
    }
}
