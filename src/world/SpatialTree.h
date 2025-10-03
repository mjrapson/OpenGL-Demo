// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/Box.h"
#include "world/World.h"

#include <memory>
#include <vector>

constexpr int MAX_TREE_STORAGE = 8;

class Ray;

struct SpatialEntity
{
    Entity entity;
    Box boundingBox;
};

struct SpatialTreeNode
{
    Box boundingBox;
    std::vector<SpatialEntity> entities;
    std::vector<std::unique_ptr<SpatialTreeNode>> children;

    bool isLeafNode() const
    {
        return children.empty();
    }
};

class SpatialTree
{
    public:
        SpatialTree(const Box& bounds, World& world);
        ~SpatialTree();

        std::vector<Entity> queryNodesInRay(const Ray& ray) const;

    private:
        void insertEntity(SpatialTreeNode* node, Entity entity, const Box& entityBoundingBox);

        void subdivide(SpatialTreeNode* node);

        void queryNodesInRange(SpatialTreeNode* node, const Ray& ray, std::vector<Entity>& hits) const;

    private:
        std::unique_ptr<SpatialTreeNode> m_root{nullptr};
};
