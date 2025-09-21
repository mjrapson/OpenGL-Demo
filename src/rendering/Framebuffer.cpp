// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Framebuffer.h"

#include "data/Texture.h"

#include <stdexcept>
#include <string>

Framebuffer::Framebuffer()
{
    glCreateFramebuffers(1, &m_handle);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_handle);
}

void Framebuffer::attachTexture(GLenum attachment, Texture& texture, GLint level) const
{
    glNamedFramebufferTexture(m_handle, attachment, texture.handle(), level);
}

void Framebuffer::attachTextureLayer(GLenum attachment, Texture& texture, GLint level, GLint layer) const
{
    glNamedFramebufferTextureLayer(m_handle, attachment, texture.handle(), level, layer);
}

void Framebuffer::setDrawBuffers(const std::vector<GLenum>& buffers) const
{
    glNamedFramebufferDrawBuffers(m_handle, buffers.size(), buffers.data());
}

void Framebuffer::setDrawBuffer(GLenum buffer) const
{
    glNamedFramebufferDrawBuffer(m_handle, buffer);
}

void Framebuffer::setReadBuffer(GLenum buffer) const
{
    glNamedFramebufferReadBuffer(m_handle, buffer);
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

void Framebuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}