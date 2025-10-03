/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/Box.h"
#include "data/MeshInstance.h"

#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

using Entity = uint32_t;

struct MeshRendererComponent
{
    std::vector<MeshInstance> meshInstances;

    MeshRendererComponent& addMeshInstance(Material* material, Mesh* mesh)
    {
        auto instance = MeshInstance{};
        instance.material = material;
        instance.mesh = mesh;
        meshInstances.push_back(instance);

        return *this;
    }

    MeshRendererComponent& addMeshInstances(const std::vector<MeshInstance>& instances)
    {
        meshInstances.insert(meshInstances.end(), instances.begin(), instances.end());
        return *this;
    }
};

struct TransformComponent
{
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    TransformComponent& setPosition(float x, float y, float z)
    {
        position = glm::vec3{x, y, z};
        return *this;
    }

    TransformComponent& setRotation(float x, float y, float z)
    {
        rotation = glm::vec3{x, y, z};
        return *this;
    }

    TransformComponent& setScale(float x, float y, float z)
    {
        scale = glm::vec3{x, y, z};
        return *this;
    }
};

struct DirectionalLightComponent
{
    glm::vec3 direction{0.0f};
    glm::vec3 color{1.0f};

    DirectionalLightComponent& setDirection(float x, float y, float z)
    {
        direction = glm::vec3{x, y, z};
        return *this;
    }

    DirectionalLightComponent& setColor(float r, float g, float b)
    {
        color = glm::vec3{r, g, b};
        return *this;
    }
};

struct PointLightComponent
{
    glm::vec3 color{1.0f};
    float radius{1.0f};

    PointLightComponent& setColor(float r, float g, float b)
    {
        color = glm::vec3{r, g, b};
        return *this;
    }

    PointLightComponent& setRadius(float r)
    {
        radius = r;
        return *this;
    }
};

class World
{
    public:
        Entity createEntity()
        {
            return nextEntity++;
        }

        void destroyEntity(Entity entity)
        {
            m_directionalLightComponents.erase(entity);
            m_meshRendererComponents.erase(entity);
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
        const auto& getAllComponents()
        {
            return getStorage<Component>();
        }

    private:
        template<typename Component>
        auto& getStorage()
        {
            if constexpr(std::is_same_v<Component, DirectionalLightComponent>)
            {
                return m_directionalLightComponents;
            }
            if constexpr(std::is_same_v<Component, MeshRendererComponent>)
            {
                return m_meshRendererComponents;
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
        std::unordered_map<Entity, DirectionalLightComponent> m_directionalLightComponents;
        std::unordered_map<Entity, MeshRendererComponent> m_meshRendererComponents;
        std::unordered_map<Entity, PointLightComponent> m_pointLightComponents;
        std::unordered_map<Entity, TransformComponent> m_transformComponents;

    private:
        Entity nextEntity{0};
};
