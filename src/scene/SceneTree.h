// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "data/Box.h"
#include "data/Ray.h"
#include "data/Sphere.h"
#include "physics/Collision.h"
#include "scene/Scene3DModel.h"

#include <memory>
#include <vector>

constexpr int MAX_TREE_DEPTH = 8;
constexpr int MAX_TREE_STORAGE = 8;
constexpr int TREE_CHILD_COUNT = 8;

class SceneTree
{
    public:
        struct RayQueryResult
        {
                Scene3DModel* object;
                float distance;
        };

        SceneTree()
            : m_region{glm::vec3{0.0f}, glm::vec3{0.0f}},
              m_depth{0}
        {
        }

        SceneTree(int depth, const Box& region)
            : m_region{region},
              m_depth{depth}

        {
        }

        ~SceneTree()
        {
            clear();
        }

        void rebuild(std::vector<Scene3DModel*> objects)
        {
            clear();

            if (objects.empty())
            {
                return;
            }

            m_region = objects.at(0)->boundingBox();

            for (auto i = 1; i < objects.size(); ++i)
            {
                m_region.expandToFit(objects.at(i)->boundingBox());
            }

            // Insert objects
            for (auto obj : objects)
            {
                insertNode(obj);
            }
        }

        void insertNode(Scene3DModel* node)
        {
            const auto boundingBox = node->boundingBox();

            if (!intersects(boundingBox))
            {
                return;
            }

            bool result = false;
            // If we are not subdivided (i.e. we are a leaf) then
            // see if we can fit the new node, otherwise we need to subdivide further
            if (isLeaf())
            {
                // See if we have space in this quadrant
                if (m_nodes.size() < MAX_TREE_STORAGE || m_depth == MAX_TREE_DEPTH)
                {
                    m_nodes.push_back(node);
                    result = true;
                }
                else
                {
                    subdivide();
                }
            }

            // If we are not a leaf, or subdivided previously to make room
            // then try and insert into our own leaves
            // Attempt all leaves incase the node overlaps quad boundaries
            if (!result)
            {
                for (auto child : m_children)
                {
                    child.insertNode(node);
                }
            }
        }

        void subdivide()
        {
            if (!m_children.empty())
            {
                return;
            }

            const auto min = m_region.min();
            const auto max = m_region.max();
            const glm::vec3 center = (min + max) * 0.5f; // Midpoint

            const auto childDepth = m_depth + 1;

            // Subdivide into 8 smaller regions
            m_children.reserve(8);
            m_children.emplace_back(childDepth, Box{glm::vec3{min.x, min.y, min.z}, center});
            m_children.emplace_back(
                childDepth, Box{glm::vec3{center.x, min.y, min.z}, glm::vec3{max.x, center.y, center.z}});
            m_children.emplace_back(
                childDepth, Box{glm::vec3{min.x, center.y, min.z}, glm::vec3{center.x, max.y, center.z}});
            m_children.emplace_back(
                childDepth, Box{glm::vec3{center.x, center.y, min.z}, glm::vec3{max.x, max.y, center.z}});
            m_children.emplace_back(
                childDepth, Box{glm::vec3{min.x, min.y, center.z}, glm::vec3{center.x, center.y, max.z}});
            m_children.emplace_back(
                childDepth, Box{glm::vec3{center.x, min.y, center.z}, glm::vec3{max.x, center.y, max.z}});
            m_children.emplace_back(
                childDepth, Box{glm::vec3{min.x, center.y, center.z}, glm::vec3{center.x, max.y, max.z}});
            m_children.emplace_back(
                childDepth, Box{glm::vec3{center.x, center.y, center.z}, glm::vec3{max.x, max.y, max.z}});

            // Distribute objects into children
            for (auto node : m_nodes)
            {
                for (auto& child : m_children)
                {
                    if (child.m_region.contains(node->boundingBox()))
                    {
                        child.insertNode(node);
                    }
                }
            }

            m_nodes.clear();
        }

        void clear()
        {
            m_children.clear();

            m_nodes.clear();
        }

        bool isLeaf() const
        {
            return m_children.size() == 0;
        }

        bool intersects(const Ray& ray) const
        {
            auto d = 0.0f;
            return collision::intersects(ray, m_region, d);
        }

        bool intersects(const Box& box) const
        {
            return m_region.intersects(box);
        }

        bool intersects(const Sphere& sphere) const
        {
            // Approximate sphere with bounding box for faster but
            // still accurate enough calculations
            const auto min = glm::vec3{
                sphere.centerPoint().x - sphere.radius(),
                sphere.centerPoint().y - sphere.radius(),
                sphere.centerPoint().z - sphere.radius()};

            const auto max = glm::vec3{
                sphere.centerPoint().x + sphere.radius(),
                sphere.centerPoint().y + sphere.radius(),
                sphere.centerPoint().z + sphere.radius()};

            const auto sphereBoundingBox = Box{min, max};

            return intersects(sphereBoundingBox);
        }

        void queryNodesInRange(std::vector<Scene3DModel*>& nodes, const Sphere& sphere) const
        {
            if (!intersects(sphere))
            {
                return;
            }

            if (isLeaf())
            {
                for (auto node : m_nodes)
                {
                    if (collision::intersects(sphere, node->boundingBox()))
                    {
                        nodes.push_back(node);
                    }
                }
            }
            else
            {
                for (auto child : m_children)
                {
                    child.queryNodesInRange(nodes, sphere);
                }
            }
        }

        void queryNodesInRange(std::vector<RayQueryResult>& nodes, const Ray& ray) const
        {
            if (!intersects(ray))
            {
                return;
            }

            if (isLeaf())
            {
                for (auto* node : m_nodes)
                {
                    auto distance = 0.0f;
                    if (collision::intersects(ray, node->boundingBox(), distance))
                    {
                        nodes.push_back(RayQueryResult{node, distance});
                    }
                }
            }
            else
            {
                for (const auto& child : m_children)
                {
                    child.queryNodesInRange(nodes, ray);
                }
            }
        }

    private:
        std::vector<SceneTree> m_children;

        Box m_region;

        std::vector<Scene3DModel*> m_nodes;

        int m_depth;
};
