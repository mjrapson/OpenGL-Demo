/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "Entity.h"
#include "world/components/BehaviourComponent.h"
#include "world/components/CameraComponent.h"
#include "world/components/DirectionalLightComponent.h"
#include "world/components/MeshRenderingComponent.h"
#include "world/components/ParticleEmitterComponent.h"
#include "world/components/PointLightComponent.h"
#include "world/components/TransformComponent.h"

#include <stdexcept>
#include <unordered_map>

class World
{
    public:
        Entity createEntity()
        {
            return nextEntity++;
        }

        void destroyEntity(Entity entity)
        {
            m_behaviourComponents.erase(entity);
            m_cameraComponents.erase(entity);
            m_directionalLightComponents.erase(entity);
            m_meshRendererComponents.erase(entity);
            m_particleEmitterComponents.erase(entity);
            m_pointLightComponents.erase(entity);
            m_transformComponents.erase(entity);
        }

        template<typename Component, typename... Args>
        Component& addComponent(Entity entity, Args&&... args)
        {
            auto& storage = getStorage<Component>();
            return storage[entity] = Component(std::forward<Args>(args)...);
        }

        template<typename Component>
        Component* getComponent(Entity entity)
        {
            auto& storage = getStorage<Component>();
            if (auto itr = storage.find(entity); itr != storage.end())
            {
                return &itr->second;
            }
            return nullptr;
        }

        template<typename Component>
        auto& getAllComponents()
        {
            return getStorage<Component>();
        }

    private:
        template<typename Component>
        auto& getStorage()
        {
            if constexpr(std::is_same_v<Component, BehaviourComponent>)
            {
                return m_behaviourComponents;
            }
            if constexpr(std::is_same_v<Component, CameraComponent>)
            {
                return m_cameraComponents;
            }
            if constexpr(std::is_same_v<Component, DirectionalLightComponent>)
            {
                return m_directionalLightComponents;
            }
            if constexpr(std::is_same_v<Component, MeshRendererComponent>)
            {
                return m_meshRendererComponents;
            }
            if constexpr(std::is_same_v<Component, ParticleEmitterComponent>)
            {
                return m_particleEmitterComponents;
            }
            if constexpr(std::is_same_v<Component, PointLightComponent>)
            {
                return m_pointLightComponents;
            }
            if constexpr(std::is_same_v<Component, TransformComponent>)
            {
                return m_transformComponents;
            }

            throw std::runtime_error("Component type not supported!");
        }

    private:
        std::unordered_map<Entity, BehaviourComponent> m_behaviourComponents;
        std::unordered_map<Entity, CameraComponent> m_cameraComponents;
        std::unordered_map<Entity, DirectionalLightComponent> m_directionalLightComponents;
        std::unordered_map<Entity, MeshRendererComponent> m_meshRendererComponents;
        std::unordered_map<Entity, ParticleEmitterComponent> m_particleEmitterComponents;
        std::unordered_map<Entity, PointLightComponent> m_pointLightComponents;
        std::unordered_map<Entity, TransformComponent> m_transformComponents;

    private:
        Entity nextEntity{0};
};
