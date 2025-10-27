// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/AssetDatabase.h"

#include <memory>

class BehaviourSystem;
class InputHandler;
class LightingSystem;
class LuaState;
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
        void initGlfw();
        void createWindow();
        void loadGl();
        void setWindowCallbacks();
        void createLuaTypes();

        void framebufferResizeCallback(int width, int height);
        void cursorPosChangeCallback(double x, double y);
        void mouseButtonPressCallback(int button, int action, int modifiers);
        void keyPressCallback(int key, int scancode, int action, int mods);
        
    private:
        std::unique_ptr<Window> m_window{nullptr};
        std::unique_ptr<Renderer> m_renderer{nullptr};
        std::unique_ptr<LuaState> m_lua{nullptr};
        std::unique_ptr<World> m_world{nullptr};
        std::unique_ptr<InputHandler> m_inputHandler{nullptr};
        std::unique_ptr<RenderSystem> m_renderSystem{nullptr};
        std::unique_ptr<BehaviourSystem> m_behaviourSystem{nullptr};
        std::unique_ptr<LightingSystem> m_lightingSystem{nullptr};
        
        AssetDatabase m_assetDb;
};
