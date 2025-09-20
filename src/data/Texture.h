// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glad/gl.h>

class Texture
{
    public:
        Texture(GLenum format, GLsizei width, GLsizei height);
        ~Texture();

        Texture(const Texture& other) = delete;
        Texture(Texture&& other) = delete;

        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) = delete;

    private:
        GLuint m_handle{0};
};