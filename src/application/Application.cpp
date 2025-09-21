// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Application.h"

#include "application/Window.h"
#include "core/Vertex.h"
#include "data/Container.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/MeshFactory.h"
#include "data/Texture.h"
#include "rendering/Camera.h"
#include "rendering/Renderer.h"
#include "scene/Scene.h"
#include "scene/Scene3DModel.h"
#include "scene/SceneDirectionalLightObject.h"
#include "scene/ScenePointLightObject.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

Application::Application()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialise glfw.");
    }

    glfwSetTime(0);

    m_window = std::make_unique<Window>("OpenGL Demo", 800, 600);
    m_window->makeCurrent();

    if (gladLoadGL(glfwGetProcAddress) == 0)
    {
        throw std::runtime_error("Failed to load OpenGL");
    }

    m_renderer = std::make_unique<Renderer>();

    // Demo assets
    auto container = std::make_unique<Container>();
    container->meshes["cube"] = MeshFactory::createCubePrimitive("cube");
    container->meshes["sphere"] = MeshFactory::createCubePrimitive("sphere");

    auto redMaterial = std::make_unique<Material>();
    redMaterial->name = "redMaterial";
    redMaterial->diffuse = glm::vec3{1.0f, 0.0f, 0.0f};
    container->materials["redMaterial"] = std::move(redMaterial);

    // Demo scene
    auto cube1 = std::make_unique<Scene3DModel>();
    cube1->setObjectName("Cube 1");
    cube1->setPosition(glm::vec3{1.0f, 1.0f, 0.0f});
    cube1->addMeshInstance(MeshInstance{
        .material = container->materials.at("redMaterial").get(), 
        .mesh = container->meshes.at("cube").get()}
    );

    auto sun = std::make_unique<SceneDirectionalLightObject>();
    sun->setObjectName("Sun");
    sun->setDirection(glm::vec3{1.0f, 0.0f, 0.0f});
    sun->setColor(glm::vec3{1.0f, 1.0f, 1.0f});

    m_scene = std::make_unique<Scene>();
    m_scene->add3DModel(std::move(cube1));
    m_scene->setDirectionalLight(std::move(sun));

    // Demo camera
    m_camera = std::make_unique<Camera>();
    m_camera->setPitch(-30.0f);
    m_camera->setPosition(glm::vec3{-20.0f, 10.0f, 0.0f});

    m_renderer->setAssets(std::move(container));
}

Application::~Application()
{
    m_scene.reset(nullptr);
    m_renderer.reset(nullptr);

    glfwTerminate();
}

void Application::run()
{
    auto deltaTime = 0.0f;
    auto currTime = 0.0f;
    auto lastTime = 0.0f;
    constexpr auto maxFps = std::chrono::duration<double, std::milli>(1000.0 / 60);

    while (!m_window->shouldClose())
    {
        const auto frameStartTime = std::chrono::high_resolution_clock::now();
        currTime = glfwGetTime();
        deltaTime = (currTime - lastTime);
        lastTime = currTime;

        glfwPollEvents();

        m_renderer->drawScene(*m_scene.get(), *m_camera.get());

        m_window->swapBuffers();

        const auto frameFinishTime = std::chrono::high_resolution_clock::now();
        const auto frameDuration = std::chrono::duration<double, std::milli>(frameFinishTime - frameStartTime);
        const auto sleepTime = std::chrono::duration<double, std::milli>(maxFps - frameDuration);

        if (sleepTime.count() > 0)
        {
            std::this_thread::sleep_for(sleepTime);
        }
    }
}
