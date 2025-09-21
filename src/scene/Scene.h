// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "scene/SceneTree.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Ray;
class SceneDirectionalLightObject;
class Scene3DModel;
class ScenePointLightObject;
class SceneSelection;

class Scene
{
    public:
        Scene();
        ~Scene();

        void setDirectionalLight(std::unique_ptr<SceneDirectionalLightObject> directionalLight);
        void add3DModel(std::unique_ptr<Scene3DModel> meshObject);
        void addPointLight(std::unique_ptr<ScenePointLightObject> pointLight);

        const std::unordered_map<std::string, std::unique_ptr<Scene3DModel>>& getScene3DModels() const;
        SceneDirectionalLightObject* getDirectionalLight() const;
        const std::vector<std::unique_ptr<ScenePointLightObject>>& getPointLights() const;

        Scene3DModel* pickWithRay(const Ray& ray) const;

        SceneSelection* getSceneSelection() const;

    private:
        void rebuildTree();

    private:
        std::unordered_map<std::string, std::unique_ptr<Scene3DModel>> m_meshObjects;
        std::unique_ptr<SceneDirectionalLightObject> m_directionalLight{nullptr};
        std::vector<std::unique_ptr<ScenePointLightObject>> m_pointLights;
        std::unique_ptr<SceneSelection> m_sceneSelection{nullptr};

        SceneTree m_renderableTree;
};
