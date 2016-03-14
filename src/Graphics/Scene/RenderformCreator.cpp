#include "RenderformCreator.hpp"
#include <algorithm>

RenderformCreator::RenderformCreator(ModelStore* modelStore, MaterialStore* matStore)
    : mMaterialStore(matStore)
    , mModelStore(modelStore)
{
}

void RenderformCreator::Update(const Scene::Updates& sceneUpdates)
{
    ParseAddNodeUpdates(sceneUpdates.newNodes);
    ParseDeleteNodeUpdates(sceneUpdates.deletedNodes);
}

const RenderformCreator::Renderform& RenderformCreator::GetRenderform() const
{
    return mRenderform; 
}

//--------------------------------------------------
// Private functions
//--------------------------------------------------
void RenderformCreator::ParseAddNodeUpdates(const std::vector<SceneNode*>& added)
{
    for (SceneNode* node : added)
    {
        // Get the transformation
        Transform& transformation = node->GetTransformation();

        // Get the model
        const std::string& modelName = node->GetModel();
        ModelDescription* mdl = (*mModelStore)[modelName];

        // Get material names per mesh
        const auto& materials = node->GetMaterials();

        for (const auto& mesh : mdl->meshes)
        {
            // Get material name
            std::string matName = materials[mesh.meshIndex];

            // Find if material already exists in renderform
            auto matIt = mRenderform.find(matName);

            // Set its parameters if it doesn't exist
            if (matIt == std::end(mRenderform))
            {
                // Get the material description
                MaterialDescription* matDesc = (*mMaterialStore)[matName];

                // Create new material
                auto& material = mRenderform[matName];

                // Diffuse
                material.diffTexId = matDesc->material.UsesDiffuseTexture() ? matDesc->material.GetDiffuseTexture() : 0;

                // Specular
                material.specTexId = matDesc->material.UsesSpecularTexture() ? matDesc->material.GetSpecularTexture() : 0;

                // Normal map
                material.useNormalMap = matDesc->material.UsesNormalMapTexture();
                material.nmapTexId = material.useNormalMap ? matDesc->material.GetNormalMapTexture() : 0;
            }

            // Create new renderform mesh and append it to material
            mRenderform[matName].meshes.push_back(
            { node
            , modelName
            , &transformation
            , mesh.vaoId
            , mesh.eboId
            , mesh.numIndices
            });
        }
    }
}

void RenderformCreator::ParseDeleteNodeUpdates(const std::vector<std::unique_ptr<SceneNode>>& deleted)
{
    for (auto& uptr : deleted)
    {
        SceneNode* node = uptr.get();

        // Get model name
        const std::string& modelName = node->GetModel();

        // Erase this model's meshes from renderform
        for (auto& p : mRenderform)
        {
            auto& meshes = p.second.meshes;
            const auto meshIt = std::find_if(std::begin(meshes), std::end(meshes),
                [&modelName](const Mesh& mesh) -> bool
                {
                    return modelName == mesh.modelName; 
                });

            if (meshIt != std::end(meshes))
                meshes.erase(meshIt);
        }
    }
}
