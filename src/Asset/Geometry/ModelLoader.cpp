#include "ModelLoader.hpp"
#include <functional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

ModelData ModelLoader::Load(const std::vector<std::uint8_t>& fileData, const char* type)
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
        return ModelData();

    // Used for creating the boundingBox
    glm::vec3 minPoint, maxPoint;

    auto processMesh = [&minPoint, &maxPoint](aiMesh* mesh, const aiScene* scene) -> MeshData
    {
        (void) scene;

        MeshData mData;

        for (std::uint32_t i = 0; i < mesh->mNumVertices; ++i)
        {
            VertexData vertexData;

            // Vertices
            const aiVector3D& vert = mesh->mVertices[i];
            vertexData.vx = vert.x;
            vertexData.vy = vert.y;
            vertexData.vz = vert.z;

            // Normals
            const aiVector3D& norm = mesh->mNormals[i];
            vertexData.nx = norm.x;
            vertexData.ny = norm.y;
            vertexData.nz = norm.z;

            if(mesh->HasTangentsAndBitangents())
            {
                // Tangents
                const aiVector3D& tan = mesh->mTangents[i];
                vertexData.tnx = tan.x;
                vertexData.tny = tan.y;
                vertexData.tnz = tan.z;
            }
            // TODO: think if we need the 'else' part
            //else {}

            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                const aiVector3D& texCoord = mesh->mTextureCoords[0][i];
                vertexData.tx = texCoord.x;
                vertexData.ty = texCoord.y;
            }
            else
            {
                vertexData.tx = 0;
                vertexData.ty = 0;
            }

            // AABB minPoint
            if (vert.x < minPoint.x)
                minPoint.x = vert.x;
            if (vert.y < minPoint.y)
                minPoint.y = vert.y;
            if (vert.z < minPoint.z)
                minPoint.z = vert.z;
            // AABB maxPoint
            if (vert.x > maxPoint.x)
                maxPoint.x = vert.x;
            if (vert.y > maxPoint.y)
                maxPoint.y = vert.y;
            if (vert.z > maxPoint.z)
                maxPoint.z = vert.z;

            // Material index
            mData.meshIndex = mesh->mMaterialIndex;

            mData.data.push_back(vertexData);
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

    std::function<ModelData(aiNode*, const aiScene*)> processNode =
    [&processMesh, &processNode](aiNode* node, const aiScene* scene) -> ModelData
    {
        ModelData model;

        // Process all the node's meshes
        for (std::uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model.meshes.push_back(processMesh(mesh, scene));
        }

        // Same for its children
        for (std::uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            ModelData childModel = processNode(node->mChildren[i], scene);
            for (auto& mesh : childModel.meshes)
                model.meshes.push_back(std::move(mesh));
        }

        return model;
    };

    ModelData model = processNode(scene->mRootNode, scene);
    model.boundingBox = AABB(minPoint, maxPoint);
    return model;
}

