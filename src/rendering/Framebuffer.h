// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glad/gl.h>

#include <optional>
#include <vector>

class Texture;

class Framebuffer
{
    public:
        Framebuffer();
        ~Framebuffer();

        Framebuffer(const Framebuffer& other) = delete;
        Framebuffer(Framebuffer&& other) = delete;

        Framebuffer& operator=(const Framebuffer& other) = delete;
        Framebuffer& operator=(Framebuffer&& other) = delete;

        void attachTexture(GLenum attachment, Texture& texture, GLint level) const;
        void attachTextureLayer(GLenum attachment, Texture& texture, GLint level, GLint layer) const;
        void setDrawBuffers(const std::vector<GLenum>& buffers) const;
        void setDrawBuffer(GLenum buffer) const;
        void setReadBuffer(GLenum buffer) const;

        void bind() const;
        void unbind() const;

        inline GLuint handle() const
        {
            return m_handle;
        }

    private:
        GLuint m_handle{0};
};
