// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>

class Texture
{
    public:
        explicit Texture(GLenum type);
        virtual ~Texture();

        Texture(const Texture& other) = delete;
        Texture(Texture&& other) = delete;

        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) = delete;

        void setMinFilter(GLenum value);
        void setMagFilter(GLenum value);
        void setWrapS(GLenum value);
        void setWrapT(GLenum value);
        void setBorderColor(const glm::vec4& color);
        void setComparisonMode(GLenum value);
        void setComparisonFunction(GLenum value);

        void writeImageData(GLsizei width, GLsizei height, const void* data);
        
        inline GLuint handle() const 
        {
            return m_handle;
        }

    private:
        GLuint m_handle{0};
};

class Texture2D : public Texture
{
    public:
        Texture2D(GLenum format, GLsizei width, GLsizei height);
};

class TextureCubeMap : public Texture
{
    public:
        TextureCubeMap(GLenum format, GLsizei width, GLsizei height);

        void writeImageData(GLsizei width, GLsizei height, GLsizei face, const void* data);

        void setWrapR(GLenum value);
};

class TextureCubeMapArray : public Texture
{
    public:
        TextureCubeMapArray(GLenum format, GLsizei width, GLsizei height, GLsizei depth);

        void setWrapR(GLenum value);
};
