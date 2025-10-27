/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "RenderSystem.h"

#include "data/Prefab.h"
#include "rendering/Renderer.h"
#include "world/World.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

RenderSystem::RenderSystem(Renderer& renderer, World& world)
    : m_renderer{renderer}
    , m_world{world}
{
}

void RenderSystem::update()
{
    for(auto& [entity, meshComponent] : m_world.getAllComponents<MeshRendererComponent>())
    {
        if(!meshComponent.prefab)
        {
            continue;
        }

        auto transformComponent = m_world.getComponent<TransformComponent>(entity);
        if(!transformComponent)
        {
            continue;
        }

        auto transformMatrix = glm::translate(glm::mat4(1.0f), transformComponent->position)
                                  * glm::toMat4(glm::quat(glm::radians(transformComponent->rotation)))
                                  * glm::scale(glm::mat4(1.0f), transformComponent->scale);

        for(auto& mesh : meshComponent.prefab->meshes())
        {
            auto cmd = DrawCommand{};
            cmd.mesh = mesh.get();
            cmd.transform = transformMatrix;
            
            m_renderer.queueDrawCommand(cmd);
        }
    }
}
