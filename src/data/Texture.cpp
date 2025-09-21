// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Texture.h"

Texture::Texture(GLenum type)
{
    glCreateTextures(type, 1, &m_handle);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_handle);
}

void Texture::setMinFilter(GLenum value)
{
    glTextureParameteri(handle(), GL_TEXTURE_MIN_FILTER, value);
}

void Texture::setMagFilter(GLenum value)
{
    glTextureParameteri(handle(), GL_TEXTURE_MAG_FILTER, value);
}

void Texture::setWrapS(GLenum value)
{
     glTextureParameteri(handle(), GL_TEXTURE_WRAP_S, value);
}

void Texture::setWrapT(GLenum value)
{
    glTextureParameteri(handle(), GL_TEXTURE_WRAP_T, value);
}

void Texture::setBorderColor(const glm::vec4& color)
{
    glTextureParameterfv(handle(), GL_TEXTURE_BORDER_COLOR, &color[0]);
}

void Texture::setComparisonMode(GLenum value)
{
    glTextureParameteri(handle(), GL_TEXTURE_COMPARE_MODE, value);
}

void Texture::setComparisonFunction(GLenum value)
{
    glTextureParameteri(handle(), GL_TEXTURE_COMPARE_FUNC, value);
}

Texture2D::Texture2D(GLenum format, GLsizei width, GLsizei height)
    : Texture(GL_TEXTURE_2D)
{
    glTextureStorage2D(handle(), 1, format, width, height);
}

TextureCubeMapArray::TextureCubeMapArray(GLenum format, GLsizei width, GLsizei height, GLsizei depth)
    : Texture(GL_TEXTURE_CUBE_MAP_ARRAY)
{
    glTextureStorage3D(handle(), 1, format, width, height, depth);
}

void TextureCubeMapArray::setWrapR(GLenum value)
{
    glTextureParameteri(handle(), GL_TEXTURE_WRAP_R, value);
}
