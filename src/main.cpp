// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <chrono>
#include <stdexcept>
#include <thread>

extern "C" {
#ifdef _WIN32
_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#else
__attribute__((visibility("default"))) unsigned long NvOptimusEnablement = 0x00000001;
__attribute__((visibility("default"))) int AmdPowerXpressRequestHighPerformance = 1;
#endif
}

int main(int /* argc */, char** /* argv */)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialise glfw.");
    }

    glfwSetTime(0);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    auto window = glfwCreateWindow(800, 600, "OpenGL Demo", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window.");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    glfwSwapBuffers(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    if (gladLoadGL(glfwGetProcAddress) == 0)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL");
    }

    auto deltaTime = 0.0f;
    auto currTime = 0.0f;
    auto lastTime = 0.0f;
    constexpr auto maxFps = std::chrono::duration<double, std::milli>(1000.0 / 60);

    while (!glfwWindowShouldClose(window))
    {
        const auto frameStartTime = std::chrono::high_resolution_clock::now();
        currTime = glfwGetTime();
        deltaTime = (currTime - lastTime);
        lastTime = currTime;

        glfwPollEvents();

        glfwSwapBuffers(window);

        const auto frameFinishTime = std::chrono::high_resolution_clock::now();
        const auto frameDuration = std::chrono::duration<double, std::milli>(frameFinishTime - frameStartTime);
        const auto sleepTime = std::chrono::duration<double, std::milli>(maxFps - frameDuration);

        if (sleepTime.count() > 0)
        {
            std::this_thread::sleep_for(sleepTime);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}