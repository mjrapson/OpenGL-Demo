// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "GltfLoader.h"

#include "core/Vertex.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/Prefab.h"
#include "data/Texture.h"
#include "loaders/TextureLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

glm::vec3 readBaseColor(tinygltf::Material& material)
{
    const auto col = material.pbrMetallicRoughness.baseColorFactor;

    if(col.empty())
    {
        return glm::vec3{1.0f, 1.0f, 1.0f};
        
    }
    
    return glm::vec3(col.at(0), col.at(1), col.at(2));  
}

std::optional<Texture*> readBaseColorTexture(tinygltf::Material& material, tinygltf::Model& model, Prefab& prefab)
{
    const auto texIndex = material.pbrMetallicRoughness.baseColorTexture.index;
    if(texIndex < 0)
    {
        return std::nullopt;
    }

    auto texture = prefab.getTexture(model.images[model.textures[texIndex].source].name);
    if(!texture)
    {
        return std::nullopt;
    }

    return texture;
}

std::vector<GLuint> readIndices(tinygltf::Primitive& primitive, tinygltf::Model& model)
{
    const auto& indexAcessor = model.accessors[primitive.indices];
    const auto& indexBufferView = model.bufferViews[indexAcessor.bufferView];
    const auto& indexBuffer = model.buffers[indexBufferView.buffer];

    auto indices = std::vector<GLuint>{};

    if(indexAcessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
    {
        const auto* indexData = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAcessor.byteOffset]);
        for (auto i = 0; i < indexAcessor.count; ++i) 
        {
            indices.push_back(static_cast<GLuint>(indexData[i]));
        }
    }
    else
    {
        const auto* indexData = reinterpret_cast<const uint32_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAcessor.byteOffset]);
        for (auto i = 0; i < indexAcessor.count; ++i) 
        {
            indices.push_back(static_cast<GLuint>(indexData[i]));
        }
    }

    return indices;
}

std::vector<Vertex> readVertices(tinygltf::Primitive& primitive, tinygltf::Model& model)
{
    const auto& posAcessor = model.accessors[primitive.attributes.at("POSITION")];
    const auto& posBufferView = model.bufferViews[posAcessor.bufferView];
    const auto& posBuffer = model.buffers[posBufferView.buffer];

    const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAcessor.byteOffset]);

    const auto& normalAcessor = model.accessors[primitive.attributes.at("NORMAL")];
    const auto& normalBufferView = model.bufferViews[normalAcessor.bufferView];
    const auto& normalBuffer = model.buffers[normalBufferView.buffer];

    const float* normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAcessor.byteOffset]);

    const auto& texAcessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
    const auto& texBufferView = model.bufferViews[texAcessor.bufferView];
    const auto& texBuffer = model.buffers[texBufferView.buffer];

    const float* texcoords = reinterpret_cast<const float*>(&texBuffer.data[texBufferView.byteOffset + texAcessor.byteOffset]);

    auto vertices = std::vector<Vertex>{};
    for (auto i = 0; i < posAcessor.count; ++i) 
    {
        auto v = Vertex{};
        v.position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
        v.normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
        v.textureUV = glm::vec2(texcoords[i * 2 + 0], 1.0f - texcoords[i * 2 + 1]);
        vertices.push_back(v);
    }

    return vertices;
}

std::unique_ptr<Prefab> loadBinaryModel(const std::filesystem::path& path)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    const auto ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);

    if (!warn.empty()) 
    {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) 
    {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) 
    {
        printf("Failed to parse glTF\n");
        return{};
    }

    auto prefab = std::make_unique<Prefab>();

    for(auto& image : model.images)
    {
        prefab->addTexture(image.name, loadTexture(image.width, image.height, image.image));
    }

    for(auto& gltfMaterial : model.materials)
    {
        auto material = std::make_unique<Material>();
        material->diffuse = readBaseColor(gltfMaterial);
        material->diffuseTexture = readBaseColorTexture(gltfMaterial, model, *prefab);     
        prefab->addMaterial(gltfMaterial.name, std::move(material));
    }

    for(auto& gltfMesh : model.meshes)
    {
        for(auto& primitive : gltfMesh.primitives)
        {
            auto mesh = std::make_unique<Mesh>();
            mesh->vertices = readVertices(primitive, model);
            mesh->indices = readIndices(primitive, model);
            mesh->material = prefab->getMaterial(model.materials[primitive.material].name);
            prefab->addMesh(std::move(mesh));   
        }
    }

    return prefab;
}

std::unique_ptr<Prefab> loadGLTFModel(const std::filesystem::path& path)
{
    if (path.extension() == ".glb")
    {
        return loadBinaryModel(path);
    }

    return nullptr;
}
