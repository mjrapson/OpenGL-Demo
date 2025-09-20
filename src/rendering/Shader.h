// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glad/gl.h>

#include <filesystem>

class Shader
{
    public:
        Shader(const std::filesystem::path& vsPath, const std::filesystem::path& fsPath);
        ~Shader();

        Shader(const Shader& other) = delete;
        Shader(Shader&& other) = delete;

        Shader& operator=(const Shader& other) = delete;
        Shader& operator=(Shader&& other) = delete;

        void bind() const;
        void unbind() const;

    private:
        GLuint m_programHandle{0};
        GLuint m_vsHandle{0};
        GLuint m_fsHandle{0};
};