// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include <glad/gl.h>

#include <vector>

template <typename DataType>
class Buffer
{
    public:
        Buffer(const std::vector<DataType>& data)
        {
            glCreateBuffers(1, &m_handle);
            glNamedBufferStorage(m_handle, data.size() * sizeof(DataType), data.data(), GL_DYNAMIC_DRAW);
        }
        ~Buffer()
        {
            glDeleteBuffers(1, &m_handle);
        }

    private:
        GLuint m_handle{0};
};