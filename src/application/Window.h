// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <string>

struct GLFWwindow;

class Window
{
    public:
        Window(const std::string& title, int width, int height);
        ~Window();

        Window(const Window& other) = delete;
        Window(Window&& other) = delete;

        Window& operator=(const Window& other) = delete;
        Window& operator=(Window&& other) = delete;

        bool shouldClose() const;

        void swapBuffers() const;

        void setFpsCounter(float fps);

        inline GLFWwindow* handle() const
        {
            return m_window;
        }

    private:
        GLFWwindow* m_window{nullptr};
        std::string m_windowTitle{};
};
