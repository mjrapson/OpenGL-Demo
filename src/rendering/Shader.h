// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glad/gl.h>

#include <filesystem>
#include <string>
#include <unordered_map>

class Texture;

class Shader
{
    public:
        Shader(const std::filesystem::path& vsPath, const std::filesystem::path& fsPath);
        ~Shader();

        Shader(const Shader& other) = delete;
        Shader(Shader&& other) = delete;

        Shader& operator=(const Shader& other) = delete;
        Shader& operator=(Shader&& other) = delete;

        void registerUniformBuffer(const std::string& name, GLsizeiptr size, GLuint index);
        void registerTextureSampler(const std::string& name, GLuint index);
        void writeUniformData(const std::string& name, GLsizeiptr size, const void* data);
        void bindTexture(const std::string& name, Texture* texture);

        void bind() const;
        void unbind() const;

    private:
        GLuint m_programHandle{0};
        GLuint m_vsHandle{0};
        GLuint m_fsHandle{0};
        std::unordered_map<std::string, GLuint> m_uniformBuffers;
        std::unordered_map<GLuint, GLuint> m_uniformSlots;
        std::unordered_map<std::string, GLuint> m_textureSamplers;
};
