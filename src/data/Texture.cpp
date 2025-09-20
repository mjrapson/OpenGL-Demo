// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Texture.h"

Texture::Texture(GLenum format, GLsizei width, GLsizei height)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);
    glTextureStorage2D(m_handle, 1, format, width, height);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_handle);
}