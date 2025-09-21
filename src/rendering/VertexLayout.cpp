// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "VertexLayout.h"

VertexLayout::VertexLayout()
{
    glCreateVertexArrays(1, &m_handle);
}

VertexLayout::~VertexLayout()
{
    glDeleteVertexArrays(1, &m_handle);
}

void VertexLayout::registerAttribute(GLuint index, GLint size, GLenum type, GLuint offset) const
{
    glVertexArrayAttribFormat(m_handle, index, size, type, GL_FALSE, offset);
    glVertexArrayAttribBinding(m_handle, index, 0);
    glEnableVertexArrayAttrib(m_handle, index);
}

void VertexLayout::bindVertexBuffer(GLuint index, GLuint bufferHandle, GLintptr offset, GLsizei stride) const
{
    glVertexArrayVertexBuffer(m_handle, index, bufferHandle, offset, stride);
}

void VertexLayout::bindElementBuffer(GLuint bufferHandle) const
{
    glVertexArrayElementBuffer(m_handle, bufferHandle);
}

void VertexLayout::bind() const
{
    glBindVertexArray(m_handle);
}

void VertexLayout::unbind() const
{
    glBindVertexArray(0);
}
