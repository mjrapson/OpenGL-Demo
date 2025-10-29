// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Application.h"

#include "application/Window.h"
#include "core/FileSystem.h"
#include "input/InputHandler.h"
#include "loaders/SceneLoader.h"
#include "rendering/Renderer.h"
#include "scripting/LuaState.h"
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

constexpr auto initialWindowWidth = 1280;
constexpr auto initialWindowHeight = 720;

Application::Application()
    : m_window{std::make_unique<Window>("OpenGL Demo", initialWindowWidth, initialWindowHeight)}
    , m_lua{std::make_unique<LuaState>()}
    , m_world{std::make_unique<World>()}
    , m_inputHandler{std::make_unique<InputHandler>()}
{
    loadGl();

    setWindowCallbacks();

    createLuaTypes();

    m_renderer = std::make_unique<Renderer>();
    m_renderer->resizeDisplay(initialWindowWidth, initialWindowHeight);
    
    m_renderSystem = std::make_unique<RenderSystem>(*m_renderer, *m_world);
    m_behaviourSystem = std::make_unique<BehaviourSystem>(*m_inputHandler, *m_world);
    m_lightingSystem = std::make_unique<LightingSystem>(*m_renderer, *m_world);

    // Demo scene
    loadScene(GetResourceDir() / "scenes/demo.json", m_assetDb, *m_world, *m_lua);
    m_renderer->setAssets(m_assetDb);
    m_behaviourSystem->init();
}

Application::~Application() = default;

void Application::run()
{
    constexpr auto maxFps = std::chrono::duration<double>(1.0 / 60.0);
    auto lastTime = std::chrono::steady_clock::now();
    auto lastFpsUpdate = std::chrono::steady_clock::now();

    auto framesSinceLastFpsUpdate = 0;

    while (!m_window->shouldClose())
    {
        const auto frameStartTime = std::chrono::steady_clock::now();
        const auto deltaTime = std::chrono::duration<double>(frameStartTime - lastTime).count();
        lastTime = frameStartTime;

        framesSinceLastFpsUpdate++;
        auto deltaTimeSinceLastFpsUpdate = std::chrono::duration<double>(frameStartTime - lastFpsUpdate).count();
        if(deltaTimeSinceLastFpsUpdate >= 1.0) 
        {
            const auto fps = framesSinceLastFpsUpdate / deltaTimeSinceLastFpsUpdate;
            m_window->setFpsCounter(fps);

            framesSinceLastFpsUpdate = 0;
            lastFpsUpdate = frameStartTime;
        }

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

        const auto frameFinishTime = std::chrono::steady_clock::now();
        const auto frameDuration = frameFinishTime - frameStartTime;
        if (frameDuration < maxFps)
        {
            std::this_thread::sleep_for(maxFps - frameDuration);
        }
    }
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

void Application::createLuaTypes()
{
    auto& state = m_lua->lua();

    sol::table keyMap = state.create_table();
    keyMap["W"] = GLFW_KEY_W;
    keyMap["A"] = GLFW_KEY_A;
    keyMap["S"] = GLFW_KEY_S;
    keyMap["D"] = GLFW_KEY_D;
    keyMap["Q"] = GLFW_KEY_Q;
    keyMap["E"] = GLFW_KEY_E;

    state["Keys"] = keyMap;

    state.new_usertype<glm::vec3>("Vec3",
        sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z,
        "cross",  [](const glm::vec3& a, const glm::vec3& b) {
            return glm::cross(a, b);
        },
        "normalize",  [](const glm::vec3& a) {
            return glm::normalize(a);
        },
        "length",  [](const glm::vec3& a) {
            return glm::length(a);
        },
        sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) {
            return a + b;
        },
        sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) {
            return a - b;
        },
        sol::meta_function::multiplication, [](const glm::vec3& a, float b) {
            return a * b;
        }
    );
    state.new_usertype<TransformComponent>("TransformComponent",
        "position", &TransformComponent::position,
        "rotation", &TransformComponent::rotation,
        "scale", &TransformComponent::scale
    );

    state.new_usertype<CameraComponent>("CameraComponent",
        "front", &CameraComponent::front,
        "up", &CameraComponent::up,
        "position", &CameraComponent::position,
        "setPosition", &CameraComponent::setPosition
    );

    auto inputHandlerType = state.new_usertype<InputHandler>("InputHandler");
    inputHandlerType["isKeyPressed"] = &InputHandler::isKeyPressed;

    auto worldType = state.new_usertype<World>("World");
    worldType["get_transform"] = [](World& w, Entity e) {
        return w.getComponent<TransformComponent>(e); 
    };
    worldType["get_camera"] = [](World& w, Entity e) { 
        return w.getComponent<CameraComponent>(e);
    };
    
}

void Application::framebufferResizeCallback(int width, int height)
{
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
    if(action == GLFW_PRESS)
    {
        m_inputHandler->setKeyPressed(key);
    }
    else if(action == GLFW_RELEASE)
    {
        m_inputHandler->setKeyReleased(key);
    }
}
