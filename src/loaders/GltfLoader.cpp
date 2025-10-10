// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#include "GltfLoader.h"

#include "core/Vertex.h"
#include "data/AssetDatabase.h"
#include "data/Material.h"
#include "data/Mesh.h"
#include "data/Texture.h"
#include "loaders/TextureLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

std::vector<MeshInstance> loadGLTFModel(const std::filesystem::path& path, AssetDatabase& assetDb)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    const auto ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

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

    auto folder = path.parent_path();

    auto instances = std::vector<MeshInstance>{};

    for(auto& image : model.images)
    {
        assetDb.textureContainer().add(image.name, loadTexture(folder / image.uri));
    }

    for(auto& mat : model.materials)
    {
        auto material = std::make_unique<Material>();

        const auto texIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
        if(texIndex >= 0)
        {
            material->diffuseTexture = assetDb.textureContainer().get(model.images[model.textures[texIndex].source].name);
        }

        if(const auto col = mat.pbrMetallicRoughness.baseColorFactor; !col.empty())
        {
            material->diffuse = glm::vec3(col.at(0), col.at(1), col.at(2));
        }
        else
        {
            material->diffuse = glm::vec3{1.0f, 1.0f, 1.0f};
        }   
        assetDb.materialContainer().add(mat.name, std::move(material));
    }

    for(auto& mesh : model.meshes)
    {
        /*
                "attributes" : {
                "POSITION" : 7,
                    "NORMAL" : 8,
                    "TEXCOORD_0" : 9,
                    "JOINTS_0" : 10,
                    "WEIGHTS_0" : 11
                },
                "indices" : 12,
                "material" : 1
        */
        auto meshData = std::make_unique<MeshData>();      

        auto& prim = mesh.primitives[0]; //for(auto& prim : mesh.primitives) // we want 1:1 - lets either take mesh.primitives[0] or change mesh structure
        
        const auto& posAcessor = model.accessors[prim.attributes.at("POSITION")];
        const auto& posBufferView = model.bufferViews[posAcessor.bufferView];
        const auto& posBuffer = model.buffers[posBufferView.buffer];

        const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAcessor.byteOffset]);

        const auto& normalAcessor = model.accessors[prim.attributes.at("NORMAL")];
        const auto& normalBufferView = model.bufferViews[normalAcessor.bufferView];
        const auto& normalBuffer = model.buffers[normalBufferView.buffer];

        const float* normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAcessor.byteOffset]);

        const auto& texAcessor = model.accessors[prim.attributes.at("TEXCOORD_0")];
        const auto& texBufferView = model.bufferViews[texAcessor.bufferView];
        const auto& texBuffer = model.buffers[texBufferView.buffer];

        const float* texcoords = reinterpret_cast<const float*>(&texBuffer.data[texBufferView.byteOffset + texAcessor.byteOffset]);

        for (size_t i = 0; i < posAcessor.count; ++i) 
        {
            Vertex v;
            v.position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
            v.normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
            v.textureUV = glm::vec2(texcoords[i * 2 + 0], 1.0f - texcoords[i * 2 + 1]);
            meshData->vertices.push_back(v);
        }

        const auto& indexAcessor = model.accessors[prim.indices];
        const auto& indexBufferView = model.bufferViews[indexAcessor.bufferView];
        const auto& indexBuffer = model.buffers[indexBufferView.buffer];

        
        if(indexAcessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
        {
            const auto* indices = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAcessor.byteOffset]);
            for (size_t i = 0; i < indexAcessor.count; ++i) 
            {
                meshData->indices.push_back(static_cast<GLuint>(indices[i]));
            }
        }
        else
        {
            const auto* indices = reinterpret_cast<const uint32_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAcessor.byteOffset]);
            for (size_t i = 0; i < indexAcessor.count; ++i) 
            {
                meshData->indices.push_back(static_cast<GLuint>(indices[i]));
            }
        }
        

        assetDb.meshContainer().add(mesh.name, std::make_unique<Mesh>(std::move(meshData)));

        instances.push_back(MeshInstance{
            .material= assetDb.materialContainer().get(model.materials[prim.material].name), 
            .mesh=assetDb.meshContainer().get(mesh.name)
        });
    }

    return instances;
}
