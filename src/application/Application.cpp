// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Application.h"

#include "application/Window.h"
#include "behaviours/CameraMoveBehaviour.h"
#include "core/FileSystem.h"
#include "core/Vertex.h"
#include "data/Container.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/MeshFactory.h"
#include "data/Texture.h"
#include "input/InputHandler.h"
#include "loaders/GltfLoader.h"
#include "loaders/TextureLoader.h"
#include "rendering/Camera.h"
#include "rendering/Renderer.h"
#include "world/systems/BehaviourSystem.h"
#include "world/systems/RenderSystem.h"
#include "world/World.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

Application::Application()
    : m_inputHandler{std::make_unique<InputHandler>()}
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialise glfw.");
    }

    glfwSetTime(0);

    m_window = std::make_unique<Window>("OpenGL Demo", 1280, 720);
    m_window->makeCurrent();

    const auto version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        throw std::runtime_error("Failed to load OpenGL");
    }

    std::cout << "Loaded OpenGL version: " << version << "\n";

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
    m_renderer->resizeDisplay(1280, 720);

    // Demo assets
    auto container = std::make_unique<Container>();
    container->meshes["cube"] = MeshFactory::createCubePrimitive("cube");
    container->meshes["sphere"] = MeshFactory::createSpherePrimitive("sphere");
    container->meshes["plane"] = MeshFactory::createPlanePrimitive("plane");

    auto wolfMeshInstances = loadGLTFModel(GetResourceDir() / "data/wolf/Wolf-Blender-2.82a.gltf", *container.get());

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
    checkerboardMaterial->name = "checkerboardMaterial";
    checkerboardMaterial->diffuseTexture = container->textures.at("checkerboard").get();
    container->materials["checkerboardMaterial"] = std::move(checkerboardMaterial);

    m_world = std::make_unique<World>();

    // Demo scene
    auto cube = m_world->createEntity();
    m_world->addComponent<TransformComponent>(cube)
        .setPosition(0.0f, 0.0f, 0.0f);
    m_world->addComponent<MeshRendererComponent>(cube)
        .addMeshInstance(
            container->materials.at("redMaterial").get(), 
            container->meshes.at("cube").get());

    auto cube2 = m_world->createEntity();
    m_world->addComponent<TransformComponent>(cube2)
        .setPosition(2.0f, 0.0f, 0.0f);
    m_world->addComponent<MeshRendererComponent>(cube2)
        .addMeshInstance(
            container->materials.at("yellowMaterial").get(), 
            container->meshes.at("cube").get());

    auto cube3 = m_world->createEntity();
    m_world->addComponent<TransformComponent>(cube3)
        .setPosition(0.0f, 0.0f, 2.0f);
    m_world->addComponent<MeshRendererComponent>(cube3)
        .addMeshInstance(
            container->materials.at("blueMaterial").get(), 
            container->meshes.at("cube").get());

    auto sphere = m_world->createEntity();
    m_world->addComponent<TransformComponent>(sphere)
        .setPosition(10.0f, 2.0f, 5.0f);
    m_world->addComponent<MeshRendererComponent>(sphere)
        .addMeshInstance(
            container->materials.at("blueMaterial").get(), 
            container->meshes.at("sphere").get());

    auto floor = m_world->createEntity();
    m_world->addComponent<TransformComponent>(floor)
        .setPosition(-8.0f, -0.5f, -8.0f)
        .setScale(100.0f, 1.0f, 100.0f);
    m_world->addComponent<MeshRendererComponent>(floor)
        .addMeshInstance(
            container->materials.at("checkerboardMaterial").get(), 
            container->meshes.at("plane").get());

    if(!wolfMeshInstances.empty()) 
    {
        auto wolf = m_world->createEntity();
        m_world->addComponent<TransformComponent>(wolf)
            .setPosition(0.0f, 0.0f, -2.0f)
            .setRotation(0.0f, -45.0f, 0.0f)
            .setScale(5.0f, 5.0f, 5.0f);
        m_world->addComponent<MeshRendererComponent>(wolf)
            .addMeshInstances(wolfMeshInstances);
    }

    auto sun = m_world->createEntity();
    m_world->addComponent<DirectionalLightComponent>(sun)
        .setDirection(1.0f, -1.0f, 1.0f)
        .setColor(0.8f, 0.8f, 0.8f);

    auto lamp1 = m_world->createEntity();
    m_world->addComponent<PointLightComponent>(lamp1)
        .setColor(0.8f, 0.8f, 0.8f)
        .setRadius(4.0f);
    m_world->addComponent<TransformComponent>(lamp1)
        .setPosition(0.0f, 2.0f, 3.0f);

    auto lamp2 = m_world->createEntity();
    m_world->addComponent<PointLightComponent>(lamp2)
        .setColor(0.8f, 0.3f, 0.8f)
        .setRadius(2.0f);
    m_world->addComponent<TransformComponent>(lamp2)
        .setPosition(-1.0f, 2.0f, -4.0f);

    auto lamp3 = m_world->createEntity();
    m_world->addComponent<PointLightComponent>(lamp3)
        .setColor(0.8f, 0.8f, 0.8f)
        .setRadius(4.0f);
    m_world->addComponent<TransformComponent>(lamp3)
        .setPosition(9.0f, 5.0f, 6.0f);

    // Demo camera
    auto camera = std::make_unique<Camera>();
    camera->setPitch(-15.0f);
    camera->setPosition(glm::vec3{-15.0f, 8.0f, 0.0f});

    // Player controller
    auto player = m_world->createEntity();

    auto& cameraComponent = m_world->addComponent<CameraComponent>(player);
    cameraComponent.camera.setPitch(-15.0f);
    cameraComponent.camera.setPosition(glm::vec3{-15.0f, 8.0f, 0.0f});

    m_world->addComponent<BehaviourComponent>(player)
        .addBehaviour(std::make_unique<CameraMoveBehaviour>());

    m_renderer->setAssets(std::move(container));

    m_renderSystem = std::make_unique<RenderSystem>(*m_renderer, *m_world);
    m_behaviourSystem = std::make_unique<BehaviourSystem>(*m_inputHandler, *m_world);
}

Application::~Application()
{
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

        m_behaviourSystem->update(deltaTime);

        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_renderSystem->draw();

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
    m_renderer->resizeDisplay(width, height);

    for(auto& [entity, cameraComponent] : m_world->getAllComponents<CameraComponent>())
    {
        cameraComponent.camera.setAspectRatio(width / height);
    }
}

void Application::cursorPosChangeCallback(double x, double y)
{
}

void Application::mouseButtonPressCallback(int button, int action, int modifiers)
{
}

void Application::keyPressCallback(int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        m_inputHandler->setKeyPressed(key);
    }
    else if(action == GLFW_RELEASE)
    {
        m_inputHandler->setKeyReleased(key);
    }
}
