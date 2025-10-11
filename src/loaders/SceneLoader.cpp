/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "SceneLoader.h"

#include "data/AssetDatabase.h"
#include "world/World.h"

// temp
#include "behaviours/OscillationAnimationBehaviour.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

using json = nlohmann::json;

glm::vec3 loadVec3(const json& json, const std::string& param1, const std::string& param2, const std::string& param3)
{
    return glm::vec3{json[param1], json[param2], json[param3]};
}

glm::vec3 loadXYZ(const json& json)
{
    return loadVec3(json, "x", "y", "z");
}

glm::vec3 loadRGB(const json& json)
{
    return loadVec3(json, "r", "g", "b");
}

void loadTransformComponent(const json& json, Entity entity, World& world)
{
    auto& transform = world.addComponent<TransformComponent>(entity);

    if(json.contains("position"))
    {
        transform.position = loadXYZ(json["position"]);
    }
    if(json.contains("rotation"))
    {
        transform.rotation = loadXYZ(json["rotation"]);
    }
    if(json.contains("scale"))
    {
        transform.scale = loadXYZ(json["scale"]);
    }
}

void loadMeshRendererComponent(const json& json, Entity entity, AssetDatabase& assetDb, World& world)
{
    auto& meshComponent = world.addComponent<MeshRendererComponent>(entity);

    for(const auto& meshInstanceJson : json["meshInstances"])
    {
        auto meshInstance = MeshInstance{};
        meshInstance.material = assetDb.materialContainer().get(meshInstanceJson["material"]);
        meshInstance.mesh =  assetDb.meshContainer().get(meshInstanceJson["mesh"]);
        meshComponent.meshInstances.push_back(meshInstance);
    }
}

void loadBehaviourComponent(const json& json, Entity entity, AssetDatabase& assetDb, World& world)
{
    auto& behaviourComponent = world.addComponent<BehaviourComponent>(entity);

    for(const auto& behaviourJson : json["behaviours"])
    {
        // TODO - store scripts (or behaviour construction functions in assetDb) and lookup
        if(behaviourJson["type"] == "OscillationAnimationBehaviour")
        {
            behaviourComponent.behaviours.push_back(std::make_unique<OscillationAnimationBehaviour>());
        }
    }
}

void loadDirectionalLightComponent(const json& json, Entity entity, World& world)
{
    auto& lightComponent = world.addComponent<DirectionalLightComponent>(entity);

    if(json.contains("direction"))
    {
        lightComponent.light.direction = loadXYZ(json["direction"]);
    }
    if(json.contains("color"))
    {
        lightComponent.light.color = loadRGB(json["color"]);
    }
}

void loadPointLightComponent(const json& json, Entity entity, World& world)
{
    auto& lightComponent = world.addComponent<PointLightComponent>(entity);

    if(json.contains("position"))
    {
        lightComponent.light.position = loadXYZ(json["position"]);
    }
    if(json.contains("color"))
    {
        lightComponent.light.color = loadRGB(json["color"]);
    }
    if(json.contains("radius"))
    {
        lightComponent.light.radius = json["radius"];
    }
}

void loadComponents(const json& json, Entity entity, AssetDatabase& assetDb, World& world)
{
    if(json.contains("TransformComponent"))
    {
        loadTransformComponent(json["TransformComponent"], entity, world);
    }
    if(json.contains("MeshRendererComponent"))
    {
        loadMeshRendererComponent(json["MeshRendererComponent"], entity, assetDb, world);
    }
    if(json.contains("BehaviourComponent"))
    {
        loadBehaviourComponent(json["BehaviourComponent"], entity, assetDb, world);
    }
    if(json.contains("DirectionalLightComponent"))
    {
        loadDirectionalLightComponent(json["DirectionalLightComponent"], entity, world);
    }
    if(json.contains("PointLightComponent"))
    {
        loadPointLightComponent(json["PointLightComponent"], entity, world);
    }
}

void loadEntity(const json& json, AssetDatabase& assetDb, World& world)
{
    const auto entity = world.createEntity();

    if(json.contains("components"))
    {
        loadComponents(json["components"], entity, assetDb, world);
    }
}

bool loadScene(const std::filesystem::path& path, AssetDatabase& assetDb, World& world)
{
    auto filestream = std::ifstream{path};

    auto sceneJson = json::parse(filestream);

    for(const auto& entityJson : sceneJson["entities"])
    {
        loadEntity(entityJson, assetDb, world);
    }

    return true;
}
