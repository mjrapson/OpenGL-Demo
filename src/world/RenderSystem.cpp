/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "RenderSystem.h"

#include "data/PointLight.h"
#include "rendering/Camera.h"
#include "rendering/Renderer.h"
#include "rendering/SceneData.h"
#include "world/World.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

RenderSystem::RenderSystem(const Renderer& renderer, World& world)
    : m_renderer{renderer}
    , m_world{world}
{
}

void RenderSystem::draw(const Camera& camera) const
{
    auto commandQueue = std::vector<DrawCommand>{};
    auto sceneData = SceneData{};
    sceneData.camera = &camera;

    const auto worldLightComponents = m_world.getAllComponents<DirectionalLightComponent>();
    if(worldLightComponents.empty())
    {
        // Currently, we must have a directional light to render, but only one
        return;
    }

    for(auto& [entity, lightComponent] : worldLightComponents)
    {
        sceneData.directionalLight.color = lightComponent.color;
        sceneData.directionalLight.direction = lightComponent.direction;
    }

    for(auto& [entity, meshComponent] : m_world.getAllComponents<MeshRendererComponent>())
    {
        auto transformComponent = m_world.getComponent<TransformComponent>(entity);
        if(!transformComponent)
        {
            continue;
        }

        auto transformMatrix = glm::translate(glm::mat4(1.0f), transformComponent->position)
                                  * glm::toMat4(glm::quat(glm::radians(transformComponent->rotation)))
                                  * glm::scale(glm::mat4(1.0f), transformComponent->scale);

        for(auto& instance : meshComponent.meshInstances)
        {
            auto cmd = DrawCommand{};
            cmd.instance = instance;
            cmd.transform = transformMatrix;
            commandQueue.push_back(cmd);
        }
    }

    for(auto& [entity, lightComponent] : m_world.getAllComponents<PointLightComponent>())
    {
        auto transformComponent = m_world.getComponent<TransformComponent>(entity);
        if(!transformComponent)
        {
            continue;
        }

        auto pointLight = PointLight{};
        pointLight.color = lightComponent.color;
        pointLight.radius = lightComponent.radius;
        pointLight.position = transformComponent->position;
        sceneData.pointLights.push_back(pointLight);
    }

    m_renderer.render(commandQueue, sceneData);
}
