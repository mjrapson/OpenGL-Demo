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
#include "rendering/Renderer.h"

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

    // Demo
    auto container = std::make_unique<Container>();
    container->meshes["cube"] = MeshFactory::createCubePrimitive("cube");
    container->meshes["sphere"] = MeshFactory::createCubePrimitive("sphere");

    m_renderer->setAssets(std::move(container));
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