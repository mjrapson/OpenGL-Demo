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

void Texture::writeImageData(GLsizei width, GLsizei height, const void* data)
{
    glTextureSubImage2D(m_handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateTextureMipmap(m_handle);
}

Texture2D::Texture2D(GLenum format, GLsizei width, GLsizei height)
    : Texture(GL_TEXTURE_2D)
{
    glTextureStorage2D(handle(), 1, format, width, height);
}

TextureCubeMap::TextureCubeMap(GLenum format, GLsizei width, GLsizei height)
    : Texture(GL_TEXTURE_CUBE_MAP)
{
    glTextureStorage2D(handle(), 1, format, width, height);
}

void TextureCubeMap::writeImageData(GLsizei width, GLsizei height, GLsizei face, const void* data)
{
    glTextureSubImage3D(handle(), 0, 0, 0, face, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void TextureCubeMap::setWrapR(GLenum value)
{
    glTextureParameteri(handle(), GL_TEXTURE_WRAP_R, value);
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
