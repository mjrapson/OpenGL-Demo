// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/Box.h"

#include <memory>
#include <string>
#include <vector>

#include <glad/gl.h>

struct Vertex;

struct MeshData
{
        std::string name{};
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
};

class Mesh
{
    public:
        explicit Mesh(std::unique_ptr<MeshData> data);

        Mesh(const Mesh& other) = delete;
        Mesh(Mesh&& other) = delete;

        Mesh& operator=(const Mesh& other) = delete;
        Mesh& operator=(Mesh&& other) = delete;

        const std::string& name() const;

        const std::vector<Vertex>& vertices() const;

        const std::vector<GLuint>& indices() const;

        const Box& boundingBox() const;

    private:
        void calculateBoundingBox();

    private:
        std::unique_ptr<MeshData> m_data;
        Box m_boundingBox;
};