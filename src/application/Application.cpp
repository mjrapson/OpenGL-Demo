// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Application.h"

#include "application/Window.h"
#include "core/FileSystem.h"
#include "core/Vertex.h"
#include "data/Container.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/MeshFactory.h"
#include "data/Texture.h"
#include "loaders/TextureLoader.h"
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

    glfwSetWindowUserPointer(m_window->handle(), this);

    glfwSetFramebufferSizeCallback(m_window->handle(), [](GLFWwindow* window, int width, int height) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->framebufferResizeCallback(width, height);
    });

    glfwSetCursorPosCallback(m_window->handle(), [](GLFWwindow* window, double x, double y) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->cursorPosChangeCallback(x, y);
    });

    glfwSetMouseButtonCallback(m_window->handle(), [](GLFWwindow* window, int button, int action, int modifiers) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->mouseButtonPressCallback(button, action, modifiers);
    });

    glfwSetKeyCallback(m_window->handle(), [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->keyPressCallback(key, scancode, action, mods);
    });

    m_renderer = std::make_unique<Renderer>();
    m_renderer->resizeDisplay(800, 600);

    // Demo assets
    auto container = std::make_unique<Container>();
    container->meshes["cube"] = MeshFactory::createCubePrimitive("cube");
    container->meshes["sphere"] = MeshFactory::createSpherePrimitive("sphere");
    container->meshes["plane"] = MeshFactory::createPlanePrimitive("plane");

    container->textures["checkerboard"] = loadTexture(GetTexturesDir() / "checkerboard.png");

    auto redMaterial = std::make_unique<Material>();
    redMaterial->name = "redMaterial";
    redMaterial->diffuse = glm::vec3{1.0f, 0.0f, 0.0f};
    container->materials["redMaterial"] = std::move(redMaterial);

    auto yellowMaterial = std::make_unique<Material>();
    yellowMaterial->name = "yellowMaterial";
    yellowMaterial->diffuse = glm::vec3{1.0f, 1.0f, 0.0f};
    container->materials["yellowMaterial"] = std::move(yellowMaterial);

    auto blueMaterial = std::make_unique<Material>();
    blueMaterial->name = "blueMaterial";
    blueMaterial->diffuse = glm::vec3{0.0f, 0.0f, 1.0f};
    container->materials["blueMaterial"] = std::move(blueMaterial);

    auto checkerboardMaterial = std::make_unique<Material>();
    checkerboardMaterial->name = "blueMaterial";
    checkerboardMaterial->diffuseTexture = container->textures.at("checkerboard").get();
    container->materials["checkerboardMaterial"] = std::move(checkerboardMaterial);

    // Demo scene
    auto cube1 = std::make_unique<Scene3DModel>();
    cube1->setObjectName("Cube 1");
    cube1->setPosition(glm::vec3{0.0f, 0.0f, 0.0f});
    cube1->addMeshInstance(MeshInstance{
        .material = container->materials.at("redMaterial").get(), 
        .mesh = container->meshes.at("cube").get()}
    );

    auto cube2 = std::make_unique<Scene3DModel>();
    cube2->setObjectName("Cube 2");
    cube2->setPosition(glm::vec3{2.0f, 0.0f, 0.0f});
    cube2->addMeshInstance(MeshInstance{
        .material = container->materials.at("yellowMaterial").get(), 
        .mesh = container->meshes.at("cube").get()}
    );

    auto cube3 = std::make_unique<Scene3DModel>();
    cube3->setObjectName("Cube 3");
    cube3->setPosition(glm::vec3{0.0f, 0.0f, 2.0f});
    cube3->addMeshInstance(MeshInstance{
        .material = container->materials.at("blueMaterial").get(), 
        .mesh = container->meshes.at("cube").get()}
    );

    auto sphere1 = std::make_unique<Scene3DModel>();
    sphere1->setObjectName("Sphere 1");
    sphere1->setPosition(glm::vec3{10.0f, 2.0f, 5.0f});
    sphere1->addMeshInstance(MeshInstance{
        .material = container->materials.at("blueMaterial").get(), 
        .mesh = container->meshes.at("sphere").get()}
    );

    auto floor = std::make_unique<Scene3DModel>();
    floor->setObjectName("Ground");
    floor->setPosition(glm::vec3{0.0f, -1.0f, 0.0f});
    floor->setScale(glm::vec3{100.0, 1.0, 100.0});
    floor->addMeshInstance(MeshInstance{
        .material = container->materials.at("checkerboardMaterial").get(), 
        .mesh = container->meshes.at("plane").get()}
    );

    auto sun = std::make_unique<SceneDirectionalLightObject>();
    sun->setObjectName("Sun");
    sun->setDirection(glm::vec3{1.0f, 0.0f, 0.0f});
    sun->setColor(glm::vec3{1.0f, 1.0f, 1.0f});

    m_scene = std::make_unique<Scene>();
    m_scene->add3DModel(std::move(cube1));
    m_scene->add3DModel(std::move(cube2));
    m_scene->add3DModel(std::move(cube3));
    m_scene->add3DModel(std::move(sphere1));
    m_scene->add3DModel(std::move(floor));
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

        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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

void Application::framebufferResizeCallback(int width, int height)
{
    m_camera->setAspectRatio(width / height);
    m_renderer->resizeDisplay(width, height);
}

void Application::cursorPosChangeCallback(double x, double y)
{
}

void Application::mouseButtonPressCallback(int button, int action, int modifiers)
{
}

void Application::keyPressCallback(int key, int scancode, int action, int mods)
{
}
