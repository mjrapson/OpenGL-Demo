/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "SceneLoader.h"

#include "core/FileSystem.h"
#include "data/AssetDatabase.h"
#include "data/Texture.h"
#include "loaders/GltfLoader.h"
#include "loaders/ScriptLoader.h"
#include "loaders/TextureLoader.h"
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

    if(json.contains("prefab"))
    {
        if(!assetDb.prefabs().contains(json["prefab"]))
        {
            return;
        }

        meshComponent.prefab = assetDb.prefabs().at(json["prefab"]).get();
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
        if(assetDb.skyboxes().contains(json["skybox"]))
        {
            cameraComponent.setSkybox(assetDb.skyboxes().at(json["skybox"]).get());
        }
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

void loadEntity(const json& json, AssetDatabase& assetDb, World& world, LuaState& lua)
{
    const auto entity = world.createEntity();

    if(json.contains("components"))
    {
        loadComponents(json["components"], entity, assetDb, world, lua);
    }
}

void loadPrefab(const json& json, AssetDatabase& assetDb)
{
    if(!json.contains("id") || !json.contains("path"))
    {
        return;
    }

    auto prefab = loadGLTFModel(GetPrefabsDir() / json["path"]);
    if(!prefab)
    {
        return;
    }

    assetDb.addPrefab(json["id"], std::move(prefab));
}

std::unique_ptr<TextureCubeMap> loadSkyboxCubemap(const json& json)
{
    auto cmInfo = CubemapFileInfo{};
    cmInfo.px = GetTexturesDir() / json["px"];
    cmInfo.py = GetTexturesDir() / json["py"];
    cmInfo.pz = GetTexturesDir() / json["pz"];
    cmInfo.nx = GetTexturesDir() / json["nx"];
    cmInfo.ny = GetTexturesDir() / json["ny"];
    cmInfo.nz = GetTexturesDir() / json["nz"];

    auto cubemap = loadCubemapTexture(cmInfo);
    cubemap->setMinFilter(GL_LINEAR);
    cubemap->setMagFilter(GL_LINEAR);
    cubemap->setWrapS(GL_CLAMP_TO_EDGE);
    cubemap->setWrapT(GL_CLAMP_TO_EDGE);
    cubemap->setWrapR(GL_CLAMP_TO_EDGE);

    return cubemap;
}

void loadSkybox(const json& json, AssetDatabase& assetDb)
{
    if(!json.contains("id") || !json.contains("textures"))
    {
        return;
    }

    auto cubemap = loadSkyboxCubemap(json["textures"]);
    if(!cubemap)
    {
        return;
    }

    auto skybox = std::make_unique<Skybox>();
    skybox->cubemapTexture = std::move(cubemap);
    assetDb.addSkybox(json["id"], std::move(skybox));
}

bool loadScene(const std::filesystem::path& path, AssetDatabase& assetDb, World& world, LuaState& lua)
{
    auto filestream = std::ifstream{path};

    auto sceneJson = json::parse(filestream);

    for(const auto& prefabJson : sceneJson["prefabs"])
    {
        loadPrefab(prefabJson, assetDb);
    }

    for(const auto& skyboxJson : sceneJson["skyboxes"])
    {
        loadSkybox(skyboxJson, assetDb);
    }

    for(const auto& entityJson : sceneJson["entities"])
    {
        loadEntity(entityJson, assetDb, world, lua);
    }

    return true;
}
