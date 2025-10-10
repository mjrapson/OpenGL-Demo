/// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

#include "core/Container.h"

class Material;
class Mesh;
class Texture;

class AssetDatabase
{
    public:
        ~AssetDatabase();
        
        Container<Material>& materialContainer();
        const Container<Material>& materialContainer() const;

        Container<Mesh>& meshContainer();
        const Container<Mesh>& meshContainer() const;

        Container<Texture>& textureContainer();
        const Container<Texture>& textureContainer() const;

        void clearAll();

    private:
        Container<Material> m_materialContainer;
        Container<Mesh> m_meshContainer;
        Container<Texture> m_textureContainer;
};
