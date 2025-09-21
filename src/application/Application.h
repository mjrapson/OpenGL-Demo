// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <memory>

class Camera;
class Renderer;
class Scene;
class Window;

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
        std::unique_ptr<Window> m_window{nullptr};
        std::unique_ptr<Renderer> m_renderer{nullptr};
        std::unique_ptr<Scene> m_scene{nullptr};
        std::unique_ptr<Camera> m_camera{nullptr};
};
