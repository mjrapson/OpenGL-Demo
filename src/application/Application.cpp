// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Application.h"

#include "application/Window.h"
#include "behaviours/CameraMoveBehaviour.h"
#include "behaviours/OscillationAnimationBehaviour.h"
#include "core/Container.h"
#include "core/FileSystem.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/MeshFactory.h"
#include "data/Texture.h"
#include "input/InputHandler.h"
#include "loaders/SceneLoader.h"
#include "loaders/TextureLoader.h"
#include "rendering/Renderer.h"
#include "world/systems/BehaviourSystem.h"
#include "world/systems/LightingSystem.h"
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
    , m_world{std::make_unique<World>()}
{
    initGlfw();

    createWindow();

    loadGl();

    setWindowCallbacks();

    loadAssets();

    // Demo scene
    loadScene(GetResourceDir() / "scenes/demo.json", m_assetDb, *m_world);
    
    m_renderer = std::make_unique<Renderer>();
    m_renderer->resizeDisplay(1280, 720);
    m_renderer->setAssets(m_assetDb);

    m_renderSystem = std::make_unique<RenderSystem>(*m_renderer, *m_world);
    m_behaviourSystem = std::make_unique<BehaviourSystem>(*m_inputHandler, *m_world);
    m_lightingSystem = std::make_unique<LightingSystem>(*m_renderer, *m_world);
}

Application::~Application()
{
    m_renderer.reset(nullptr);

    m_assetDb.clearAll();

    glfwTerminate();
}

void Application::run()
{
    auto deltaTime = 0.0f;
    auto currTime = 0.0f;
    auto lastTime = 0.0f;
    constexpr auto maxFps = std::chrono::duration<double, std::milli>(1000.0 / 60);

    m_behaviourSystem->init();

    while (!m_window->shouldClose())
    {
        const auto frameStartTime = std::chrono::high_resolution_clock::now();
        currTime = glfwGetTime();
        deltaTime = (currTime - lastTime);
        lastTime = currTime;

        glfwPollEvents();

        m_behaviourSystem->update(deltaTime);

        m_renderer->beginFrame();

        m_lightingSystem->update();

        m_renderSystem->update();

        for(auto& [entity, cameraComponent] : m_world->getAllComponents<CameraComponent>())
        {
            m_renderer->render(cameraComponent.camera());
        }

        m_renderer->endFrame();

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

void Application::initGlfw()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialise glfw.");
    }

    glfwSetTime(0);
}

void Application::createWindow()
{
    m_window = std::make_unique<Window>("OpenGL Demo", 1280, 720);
    m_window->makeCurrent();
}

void Application::loadGl()
{
    const auto version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        throw std::runtime_error("Failed to load OpenGL");
    }

    std::cout << "Loaded OpenGL version: " << version << "\n";
}

void Application::setWindowCallbacks()
{
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
}

void Application::loadAssets()
{
    m_assetDb.meshContainer().add("cube", MeshFactory::createCubePrimitive());
    m_assetDb.meshContainer().add("sphere", MeshFactory::createSpherePrimitive());
    m_assetDb.meshContainer().add("plane", MeshFactory::createPlanePrimitive());

    m_assetDb.textureContainer().add("checkerboard", 
        loadTexture(GetTexturesDir() / "checkerboard.png"));

    m_assetDb.materialContainer().add("red", std::make_unique<Material>(Material{
        .diffuse = glm::vec3{1.0f, 0.0f, 0.0f}}));
    m_assetDb.materialContainer().add("yellow", std::make_unique<Material>(Material{
        .diffuse = glm::vec3{1.0f, 1.0f, 0.0f}}));
    m_assetDb.materialContainer().add("blue", std::make_unique<Material>(Material{
        .diffuse = glm::vec3{0.0f, 0.0f, 1.0f}}));

    m_assetDb.materialContainer().add("checkerboard", std::make_unique<Material>(Material{
        .diffuseTexture = m_assetDb.textureContainer().get("checkerboard")}));
}

void Application::framebufferResizeCallback(int width, int height)
{
    m_renderer->resizeDisplay(width, height);

    for(auto& [entity, cameraComponent] : m_world->getAllComponents<CameraComponent>())
    {
        cameraComponent.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
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
