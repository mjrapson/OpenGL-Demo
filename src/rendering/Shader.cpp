// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Shader.h"

#include "data/Texture.h"

#include <fstream>
#include <stdexcept>
#include <sstream>

std::string readFile(const std::filesystem::path& path)
{
    auto stream = std::ifstream(path);
    if (stream.is_open() == false)
    {
        return std::string{};
    }

    auto buffer = std::stringstream{};
    buffer << stream.rdbuf();
    return buffer.str();
}

unsigned int loadShader(const std::filesystem::path& path, unsigned int type)
{
    const auto id = glCreateShader(type);
    const auto source = readFile(path);
    const auto rawSource = source.c_str();
    glShaderSource(id, 1, &rawSource, nullptr);
    glCompileShader(id);

    auto success = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        throw std::runtime_error{"Failed to load shader: " + std::string{infoLog}};
    }

    return id;
}

Shader::Shader(const std::filesystem::path& vsPath, const std::filesystem::path& fsPath)
{
    m_programHandle = glCreateProgram();

    m_vsHandle = loadShader(vsPath, GL_VERTEX_SHADER);
    glAttachShader(m_programHandle, m_vsHandle);

    m_fsHandle = loadShader(fsPath, GL_FRAGMENT_SHADER);
    glAttachShader(m_programHandle, m_fsHandle);

    glLinkProgram(m_programHandle);

    auto isLinked = GL_FALSE;
    glGetProgramiv(m_programHandle, GL_LINK_STATUS, &isLinked);

    if (isLinked == GL_FALSE)
    {
        GLchar msg[512];
        glGetProgramInfoLog(m_programHandle, 512, nullptr, msg);
        throw std::runtime_error{"Failed to link shader: " + std::string{msg}};
    }
}

Shader::~Shader()
{
    glDetachShader(m_programHandle, m_vsHandle);
    glDeleteShader(m_vsHandle);

    glDetachShader(m_programHandle, m_fsHandle);
    glDeleteShader(m_fsHandle);

    glDeleteProgram(m_programHandle);
}

void Shader::registerUniformBuffer(const std::string& name, GLsizeiptr size, GLuint index)
{
    auto ubo = GLuint{0};
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo);

    const auto blockIndex = glGetUniformBlockIndex(m_programHandle, name.c_str());
    glUniformBlockBinding(m_programHandle, blockIndex, index);

    m_uniformBuffers[name] = ubo;
    m_uniformSlots[index] = ubo;
}

void Shader::registerTextureSampler(const std::string& name, GLuint index)
{
    m_textureSamplers[name] = index;
}

void Shader::writeUniformData(const std::string& name, GLsizeiptr size, const void* data)
{
    const auto ubo = m_uniformBuffers.at(name);
    glNamedBufferSubData(ubo, 0, size, data);
}

void Shader::bindTexture(const std::string& name, Texture* texture)
{
    const auto slot = m_textureSamplers.at(name);
    glBindTextureUnit(slot, texture->handle());
}

void Shader::bind() const
{
    glUseProgram(m_programHandle);

    for (const auto& [slot, ubo] : m_uniformSlots)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, slot, ubo);
    }
}

void Shader::unbind() const
{
    glUseProgram(0);
}
