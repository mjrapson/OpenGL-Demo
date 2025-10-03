// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <memory>
#include <unordered_map>

class Camera;
class Renderer;
class RenderSystem;
class Window;
class World;

class Application
{
    public:
        Application();
        ~Application();

        Application(const Application& other) = delete;
        Application(Application&& other) = delete;

        Application& operator=(const Application& other) = delete;
        Application& operator=(Application&& other) = delete;

        void run();

    private:
        void framebufferResizeCallback(int width, int height);
        void cursorPosChangeCallback(double x, double y);
        void mouseButtonPressCallback(int button, int action, int modifiers);
        void keyPressCallback(int key, int scancode, int action, int mods);

        void updateCamera(float deltaTime);

    private:
        std::unique_ptr<Window> m_window{nullptr};
        std::unique_ptr<Renderer> m_renderer{nullptr};
        std::unique_ptr<Camera> m_camera{nullptr};
        std::unique_ptr<World> m_world{nullptr};

        std::unique_ptr<RenderSystem> m_renderSystem{nullptr};

        std::unordered_map<int, bool> m_keyState;
};
