/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "SpatialTree.h"

#include "data/Prefab.h"
#include "data/Ray.h"
#include "physics/Collision.h"

#include <array>

SpatialTree::SpatialTree(const Box& bounds, World& world)
{
    m_root = std::make_unique<SpatialTreeNode>();
    m_root->boundingBox = bounds;

    for(auto& [entity, meshComponent] : world.getAllComponents<MeshRendererComponent>())
    {
        if(!meshComponent.prefab)
        {
            continue;
        }

        auto transformComponent = world.getComponent<TransformComponent>(entity);
        if(!transformComponent)
        {
            continue;
        }

        auto prefab = meshComponent.prefab;

        auto entityBB = prefab->boundingBox();

        insertEntity(m_root.get(), entity, entityBB);
    }
}

std::vector<Entity> SpatialTree::queryNodesInRay(const Ray& ray) const
{
    auto hits = std::vector<Entity>{};
    queryNodesInRange(m_root.get(), ray, hits);
    return hits;
}

void SpatialTree::insertEntity(SpatialTreeNode* node, Entity entity, const Box& entityBoundingBox)
{
    if (!node->boundingBox.intersects(entityBoundingBox))
    {
        return;
    }

    if(node->isLeafNode() && node->entities.size() < MAX_TREE_STORAGE)
    {
        node->entities.push_back({entity, entityBoundingBox});
        return;
    }

    if(node->isLeafNode())
    {
        subdivide(node);
    }

    for(auto& child : node->children)
    {
        insertEntity(child.get(), entity, entityBoundingBox);
    }
}

SpatialTree::~SpatialTree() = default;


void SpatialTree::subdivide(SpatialTreeNode* node)
{
    if (!node->children.empty())
    {
        return;
    }

    const auto min = node->boundingBox.min();
    const auto max = node->boundingBox.max();
    const glm::vec3 center = (min + max) * 0.5f;

    auto subdividedRegions = std::array<Box, 8>{};
    subdividedRegions[0] = Box{glm::vec3{min.x, min.y, min.z}, center};
    subdividedRegions[1] = Box{glm::vec3{center.x, min.y, min.z}, glm::vec3{max.x, center.y, center.z}};
    subdividedRegions[2] = Box{glm::vec3{min.x, center.y, min.z}, glm::vec3{center.x, max.y, center.z}};
    subdividedRegions[3] = Box{glm::vec3{center.x, center.y, min.z}, glm::vec3{max.x, max.y, center.z}};
    subdividedRegions[4] = Box{glm::vec3{min.x, min.y, center.z}, glm::vec3{center.x, center.y, max.z}};
    subdividedRegions[5] = Box{glm::vec3{center.x, min.y, center.z}, glm::vec3{max.x, center.y, max.z}};
    subdividedRegions[6] = Box{glm::vec3{min.x, center.y, center.z}, glm::vec3{center.x, max.y, max.z}};
    subdividedRegions[7] = Box{glm::vec3{center.x, center.y, center.z}, glm::vec3{max.x, max.y, max.z}};

    for(auto i = 0; i < 8; ++i)
    {
        node->children[i] = std::make_unique<SpatialTreeNode>();
        node->children[i]->boundingBox = subdividedRegions[i];
    }

    // Distribute objects into children
    for (auto entity : node->entities)
    {
        for(auto& child : node->children)
        {
            insertEntity(child.get(), entity.entity, entity.boundingBox);
        }
    }

    node->entities.clear();
}

void SpatialTree::queryNodesInRange(SpatialTreeNode* node, const Ray& ray, std::vector<Entity>& hits) const
{
    auto distance = 0.0f;
    if (!collision::intersects(ray, node->boundingBox, distance))
    {
        return;
    }

    if (node->isLeafNode())
    {
        for (const auto& entity : node->entities)
        {
            if (collision::intersects(ray, entity.boundingBox, distance))
            {
                hits.push_back(entity.entity);
            }
        }
    }
    else
    {
        for (const auto& child : node->children)
        {
            queryNodesInRange(child.get(), ray, hits);
        }
    }
}
