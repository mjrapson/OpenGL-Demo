// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "MeshFactory.h"

#include "core/Vertex.h"
#include "data/Mesh.h"

std::unique_ptr<Mesh> MeshFactory::createCubePrimitive(const std::string& name)
{
    auto meshData = std::make_unique<MeshData>();
    meshData->name = name;
    meshData->vertices = {
        // Front face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},  // Bottom-right
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},   // Top-right
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // Top-left

        // Back face
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // Bottom-right
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // Top-right
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // Top-left

        // Left face
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // Bottom-left
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // Top-left
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // Top-right
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom-right

        // Right face
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // Top-left
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // Top-right
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // Bottom-right

        // Top face
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // Bottom-right
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},   // Top-right
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // Top-left

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // Bottom-left
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // Bottom-right
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},   // Top-right
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},  // Top-left
    };

    meshData->indices = {
        // Front face
        0,
        1,
        2,
        2,
        3,
        0,
        // Back face
        4,
        5,
        6,
        6,
        7,
        4,
        // Left face
        8,
        9,
        10,
        10,
        11,
        8,
        // Right face
        12,
        13,
        14,
        14,
        15,
        12,
        // Top face
        16,
        17,
        18,
        18,
        19,
        16,
        // Bottom face
        20,
        21,
        22,
        22,
        23,
        20,
    };

    return std::make_unique<Mesh>(std::move(meshData));
}

std::unique_ptr<Mesh> MeshFactory::createSpherePrimitive(const std::string& name)
{
    const auto segments = 16;
    const auto radius = 1.0f;
    const auto pi = 3.14159265359f;
    const auto twoPi = 2.0f * pi;
    const auto segmentWidth = twoPi / segments;
    const auto segmentHeight = pi / segments;

    auto meshData = std::make_unique<MeshData>();
    meshData->name = name;

    for (auto y = 0; y <= segments; y++)
    {
        for (auto x = 0; x <= segments; x++)
        {
            const auto xSegment = static_cast<float>(x);
            const auto ySegment = static_cast<float>(y);
            const auto xPos = std::cos(xSegment * segmentWidth) * std::sin(ySegment * segmentHeight);
            const auto yPos = std::cos(ySegment * segmentHeight);
            const auto zPos = std::sin(xSegment * segmentWidth) * std::sin(ySegment * segmentHeight);

            Vertex vertex;
            vertex.position = glm::vec3{xPos, yPos, zPos} * radius;
            vertex.normal = glm::vec3{xPos, yPos, zPos};
            vertex.textureUV = glm::vec2{xSegment / segments, ySegment / segments};
            meshData->vertices.push_back(vertex);
        }
    }

    for (auto y = 0; y < segments; y++)
    {
        for (auto x = 0; x < segments; x++)
        {
            meshData->indices.push_back((y + 1) * (segments + 1) + x);
            meshData->indices.push_back(y * (segments + 1) + x);
            meshData->indices.push_back(y * (segments + 1) + x + 1);

            meshData->indices.push_back((y + 1) * (segments + 1) + x);
            meshData->indices.push_back(y * (segments + 1) + x + 1);
            meshData->indices.push_back((y + 1) * (segments + 1) + x + 1);
        }
    }

    return std::make_unique<Mesh>(std::move(meshData));
}

std::unique_ptr<Mesh> MeshFactory::createPlanePrimitive(const std::string& name)
{
    float halfWidth = 0.5f;
    float halfDepth = 0.5f;

    auto meshData = std::make_unique<MeshData>();
    meshData->name = name;
    meshData->vertices = {
        {{-halfWidth, 0.0f, -halfDepth}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{halfWidth, 0.0f, -halfDepth}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{halfWidth, 0.0f, halfDepth}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-halfWidth, 0.0f, halfDepth}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}};

    meshData->indices = {0, 1, 2, 2, 3, 0};

    return std::make_unique<Mesh>(std::move(meshData));
}
