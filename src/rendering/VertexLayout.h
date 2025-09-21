// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glad/gl.h>

class VertexLayout
{
public:
        VertexLayout();
        ~VertexLayout();

        VertexLayout(const VertexLayout& other) = delete;
        VertexLayout(VertexLayout&& other) = delete;

        VertexLayout& operator=(const VertexLayout& other) = delete;
        VertexLayout& operator=(VertexLayout&& other) = delete;

        void registerAttribute(GLuint index, GLint size, GLenum type, GLuint offset) const;
        void bindVertexBuffer(GLuint index, GLuint bufferHandle, GLintptr offset, GLsizei stride) const;
        void bindElementBuffer(GLuint bufferHandle) const;

        void bind() const;
        void unbind() const;

    private:
        GLuint m_handle{0};
};