/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "Prefab.h"

#include "data/Mesh.h"

void Prefab::addMaterial(const std::string& name, std::unique_ptr<Material> material)
{
    if(!material)
    {
        return;
    }

    m_materials[name] = std::move(material);
}

void Prefab::addMesh(std::unique_ptr<Mesh> mesh)
{
    if(!mesh)
    {
        return;
    }

    m_boundingBox.expandToFit(Box::enclose(mesh->vertices));

    m_meshes.push_back(std::move(mesh));
}

void Prefab::addTexture(const std::string& name, std::unique_ptr<Texture> texture)
{
    if(!texture)
    {
        return;
    }

     m_textures[name] = std::move(texture);
}

Material* Prefab::getMaterial(const std::string& name) const
{
    if(!m_materials.contains(name))
    {
        return nullptr;
    }

    return m_materials.at(name).get();
}

Texture* Prefab::getTexture(const std::string& name) const
{
    if(!m_textures.contains(name))
    {
        return nullptr;
    }

    return m_textures.at(name).get();
}

const std::vector<std::unique_ptr<Mesh>>& Prefab::meshes() const
{
    return m_meshes;
}

const Box& Prefab::boundingBox() const
{
    return m_boundingBox;
}
