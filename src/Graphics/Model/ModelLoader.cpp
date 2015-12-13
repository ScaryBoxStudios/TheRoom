#include "ModelLoader.hpp"
#include <functional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model ModelLoader::Load(std::vector<std::uint8_t> fileData, const char* type)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(
                                        fileData.data(),
                                        fileData.size(),
                                        aiProcess_Triangulate |
                                        aiProcess_FlipUVs |
                                        aiProcess_GenNormals |
                                        aiProcess_CalcTangentSpace |
                                        aiProcess_SortByPType |
                                        aiProcess_JoinIdenticalVertices |
                                        aiProcess_ImproveCacheLocality,
                                        type);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        return Model();

    auto processMesh = [](aiMesh* mesh, const aiScene* scene) -> Mesh
    {
        (void) scene;

        Mesh mData;
        for (std::uint32_t i = 0; i < mesh->mNumVertices; ++i)
        {
            // Vertices
            const aiVector3D& vert = mesh->mVertices[i];
            mData.vertices.push_back(vert.x);
            mData.vertices.push_back(vert.y);
            mData.vertices.push_back(vert.z);

            // Normals
            const aiVector3D& norm = mesh->mNormals[i];
            mData.normals.push_back(norm.x);
            mData.normals.push_back(norm.y);
            mData.normals.push_back(norm.z);

            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                const aiVector3D& texCoord = mesh->mTextureCoords[0][i];
                mData.texCoords.push_back(texCoord.x);
                mData.texCoords.push_back(texCoord.y);
            }
        }

        // Indices
        for (std::uint32_t i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (std::uint32_t j = 0; j < face.mNumIndices; ++j)
                mData.indices.push_back(face.mIndices[j]);
        }

        // Material
        if (scene->HasMaterials())
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            if (material)
            {
                auto texTypes = {aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS, aiTextureType_NONE};
                for (auto type : texTypes)
                {
                    std::uint32_t texCount = material->GetTextureCount(type);
                    for (std::uint32_t i = 0; i < texCount; ++i)
                    {
                        aiString str;
                        material->GetTexture(type, i, &str);
                        // TODO
                    }
                }
            }
        }

        return mData;
    };

    std::function<Model(aiNode*, const aiScene*)> processNode =
    [&processMesh, &processNode](aiNode* node, const aiScene* scene) -> Model
    {
        Model model;

        // Process all the node's meshes
        for (std::uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model.meshes.push_back(processMesh(mesh, scene));
        }

        // Same for its children
        for (std::uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            Model childModel = processNode(node->mChildren[i], scene);
            for (auto& mesh : childModel.meshes)
                model.meshes.push_back(std::move(mesh));
        }

        return model;
    };

    return processNode(scene->mRootNode, scene);
}

