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

glm::vec3 loadXYZ(const json& json)
{
    auto vec3 = glm::vec3{0.0f};
    vec3.x = json["x"];
    vec3.y = json["y"];
    vec3.z = json["z"];

    return vec3;
}

void loadTransformComponent(const json& json, Entity entity, AssetDatabase& assetDb, World& world)
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

void loadComponents(const json& json, Entity entity, AssetDatabase& assetDb, World& world)
{
    if(json.contains("TransformComponent"))
    {
        loadTransformComponent(json["TransformComponent"], entity, assetDb, world);
    }
    if(json.contains("MeshRendererComponent"))
    {
        loadMeshRendererComponent(json["MeshRendererComponent"], entity, assetDb, world);
    }
    if(json.contains("BehaviourComponent"))
    {
        loadBehaviourComponent(json["BehaviourComponent"], entity, assetDb, world);
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
