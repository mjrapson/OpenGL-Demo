// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Scene.h"

#include "scene/Scene3DModel.h"
#include "scene/SceneDirectionalLightObject.h"
#include "scene/ScenePointLightObject.h"
#include "scene/SceneSelection.h"

Scene::Scene()
    : m_sceneSelection{std::make_unique<SceneSelection>()}
{
}

Scene::~Scene() = default;

void Scene::setDirectionalLight(std::unique_ptr<SceneDirectionalLightObject> directionalLight)
{
    m_directionalLight = std::move(directionalLight);
}

void Scene::add3DModel(std::unique_ptr<Scene3DModel> meshObject)
{
    // m_renderableTree.insertNode(meshObject.get());

    m_meshObjects[meshObject->objectName()] = std::move(meshObject);

    rebuildTree();
}

void Scene::addPointLight(std::unique_ptr<ScenePointLightObject> pointLight)
{
    m_pointLights.push_back(std::move(pointLight));
}

SceneDirectionalLightObject* Scene::getDirectionalLight() const
{
    return m_directionalLight.get();
}

const std::unordered_map<std::string, std::unique_ptr<Scene3DModel>>& Scene::getScene3DModels() const
{
    return m_meshObjects;
}

const std::vector<std::unique_ptr<ScenePointLightObject>>& Scene::getPointLights() const
{
    return m_pointLights;
}

SceneSelection* Scene::getSceneSelection() const
{
    return m_sceneSelection.get();
}

// Scene3DModel* Scene::pickWithRay(const Ray& ray) const
// {
//     std::vector<SceneTree::RayQueryResult> results;
//     m_renderableTree.queryNodesInRange(results, ray);

//     if (results.empty())
//     {
//         return nullptr;
//     }

//     auto smallestDistance = std::numeric_limits<float>::infinity();
//     Scene3DModel* closestObject = nullptr;

//     for (const auto& hit : results)
//     {
//         if (hit.distance < smallestDistance)
//         {
//             smallestDistance = hit.distance;
//             closestObject = hit.object;
//         }
//     }

//     return closestObject;
// }

void Scene::rebuildTree()
{
    // std::vector<Scene3DModel*> meshObjects;
    // for (auto& object : m_meshObjects)
    // {
    //     meshObjects.push_back(object.second.get());
    // }

    // m_renderableTree.rebuild(meshObjects);
}