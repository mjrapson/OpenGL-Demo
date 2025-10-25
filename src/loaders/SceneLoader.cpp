/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "SceneLoader.h"

#include "core/FileSystem.h"
#include "data/AssetDatabase.h"
#include "loaders/GltfLoader.h"
#include "loaders/ScriptLoader.h"
#include "scripting/LuaScript.h"
#include "world/LuaBehaviour.h"
#include "world/World.h"

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

void loadBehaviourComponent(const json& json, Entity entity, AssetDatabase& assetDb, World& world, LuaState& lua)
{
    auto& behaviourComponent = world.addComponent<BehaviourComponent>(entity);

    for(const auto& behaviourJson : json["behaviours"])
    {
        if(behaviourJson["type"] == "lua")
        {
            const auto path = GetScriptsDir() / behaviourJson["script"];

            behaviourComponent.behaviours.push_back(std::make_unique<LuaBehaviour>(loadScript(path, lua)));
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

void loadCameraComponent(const json& json, Entity entity, AssetDatabase& assetDb, World& world)
{
    auto& cameraComponent = world.addComponent<CameraComponent>(entity);

    if(json.contains("position"))
    {
        cameraComponent.setPosition(loadXYZ(json["position"]));
    }
    if(json.contains("pitch"))
    {
        cameraComponent.setPitch(json["pitch"]);
    }
    if(json.contains("skybox"))
    {
        cameraComponent.setSkyboxTexture(assetDb.textureContainer().get(json["skybox"]));
    }
}

void loadComponents(const json& json, Entity entity, AssetDatabase& assetDb, World& world, LuaState& lua)
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
        loadBehaviourComponent(json["BehaviourComponent"], entity, assetDb, world, lua);
    }
    if(json.contains("DirectionalLightComponent"))
    {
        loadDirectionalLightComponent(json["DirectionalLightComponent"], entity, world);
    }
    if(json.contains("PointLightComponent"))
    {
        loadPointLightComponent(json["PointLightComponent"], entity, world);
    }
    if(json.contains("CameraComponent"))
    {
        loadCameraComponent(json["CameraComponent"], entity, assetDb, world);
    }
}

void loadPrefab(const std::string& path, Entity entity, AssetDatabase& assetDb, World& world)
{
    const auto meshInstances = loadGLTFModel(GetResourceDir() / path, assetDb);
    if(meshInstances.empty())
    {
        return;
    }

    auto& mesh = world.addComponent<MeshRendererComponent>(entity);
    for(const auto& instance : meshInstances)
    {
        mesh.meshInstances.push_back(instance);
    }
}

void loadEntity(const json& json, AssetDatabase& assetDb, World& world, LuaState& lua)
{
    const auto entity = world.createEntity();

    if(json.contains("components"))
    {
        loadComponents(json["components"], entity, assetDb, world, lua);
    }
    if(json.contains("prefab"))
    {
        loadPrefab(json["prefab"], entity, assetDb, world);
    }
}

bool loadScene(const std::filesystem::path& path, AssetDatabase& assetDb, World& world, LuaState& lua)
{
    auto filestream = std::ifstream{path};

    auto sceneJson = json::parse(filestream);

    for(const auto& entityJson : sceneJson["entities"])
    {
        loadEntity(entityJson, assetDb, world, lua);
    }

    return true;
}
