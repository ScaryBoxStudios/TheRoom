#include "SceneFactory.hpp"

#include <algorithm>
#include <assert.h>
#include "../../Asset/Image/ImageLoader.hpp"
#include "../../Asset/Geometry/ModelLoader.hpp"
#include "../../Util/FileLoad.hpp"

SceneFactory::SceneFactory(TextureStore* tStore, ModelStore* mdlStore, MaterialStore* matStore, ScreenContext::FileDataCache* fdc)
    : mTextureStore(tStore)
    , mModelStore(mdlStore)
    , mMaterialStore(matStore)
    , mFileDataCache(fdc)
{
}

std::unique_ptr<Scene> SceneFactory::CreateFromSceneFile(const Properties::SceneFile& sceneFile)
{
    LoadTextures(sceneFile.extraMaterials.textures);
    LoadMaterials(sceneFile.extraMaterials.materials);
    LoadGeometries(sceneFile.extraModels.geometries);

    return CreateScene(sceneFile.scene, sceneFile.extraModels.models);
}

void SceneFactory::LoadTextures(const std::vector<Properties::Texture>& textures)
{
    // The ImageLoader object
    ImageLoader imageLoader;

    for(const auto& t : textures)
    {
        // Ignore that texture if it has already been loaded
        if((*mTextureStore)[t.id.data] != nullptr)
            continue;

        std::string ext = t.url.substr(t.url.find_last_of(".") + 1);
        RawImage pb = imageLoader.Load(*(*mFileDataCache)[t.url], ext);
        mTextureStore->Load(t.id.data, std::move(pb));
    }
}

void SceneFactory::LoadMaterials(const std::vector<Properties::Material>& materials)
{
    for(auto& m : materials)
    {
        // Ignore that material if it has already been loaded
        if((*mMaterialStore)[m.id.data] != nullptr)
            continue;

        Material newMat;

        if(m.dmap.data.compare("") != 0)
            newMat.SetDiffuseTexture((*mTextureStore)[m.dmap.data]->texId);

        // Add specular
        if(m.smap.data.compare("") != 0)
            newMat.SetSpecularTexture((*mTextureStore)[m.smap.data]->texId);

        // Add normal map
        if(m.nmap.data.compare("") != 0)
            newMat.SetNormalMapTexture((*mTextureStore)[m.nmap.data]->texId);

        // Add color
        newMat.SetDiffuseColor(glm::vec3(m.color.r, m.color.g, m.color.b));
        //newMat.SetSpecularColor(glm::vec3(m.specular.r, m.specular.g, m.specular.b));
        newMat.SetEmissiveColor(glm::vec3(m.emissive.r, m.emissive.g, m.emissive.b));
        newMat.SetRoughness(m.roughness);
        newMat.SetFresnel(m.reflectivity);
        newMat.SetMetallic(m.metallic);
        newMat.SetTransparency(m.transparency);

        mMaterialStore->Load(m.id.data, newMat);
    }
}

void SceneFactory::LoadGeometries(const std::vector<Properties::Geometry>& geometries)
{
    // Model loader instance
    ModelLoader modelLoader;

    for(auto& geometry : geometries)
    {
        // Ignore that geometry if it has alredy been loaded
        if((*mModelStore)[geometry.id.data] != nullptr)
            continue;

        // Check if path not empty
        if(geometry.url.empty())
            continue;

        // Check if model is already loaded and if not, load it now!
        if((*mFileDataCache).find(geometry.url) == std::end(*mFileDataCache))
        {
            mFileDataCache->emplace(geometry.url, FileLoad<BufferType>(geometry.url));
            if(!(*mFileDataCache)[geometry.url])
                throw std::runtime_error("Couldn't load file (" + geometry.url + ")");
        }

        // Find the file
        auto& file = (*mFileDataCache)[geometry.url];

        // Find file extension
        std::string ext = geometry.url.substr(geometry.url.find_last_of(".") + 1);

        // Load model
        ModelData model = modelLoader.Load(*file, ext.c_str());
        if(model.meshes.size() == 0)
            throw std::runtime_error("Couldn't load model (" + geometry.url + ")");

        mModelStore->Load(geometry.id.data, std::move(model));

        //(*mModelStore)[geometry.uuid.ToString()]->material = *(*mMaterialStore)[m.material];
    }
}

void SceneFactory::LoadSceneNode(
    Scene* const scene,
    const Properties::SceneNode& node,
    const std::vector<Properties::Model>& models)
{
    // Find the right model
    const auto modelIt = std::find_if(
        std::begin(models),
        std::end(models),
        [&node](const Properties::Model& model) -> bool { return node.model == model.id; });

    // Assert if model not found (unlikely because of Properties Validation stage)
    assert(modelIt != std::end(models));

    // Find the right geometry
    ModelDescription* model = (*mModelStore)[modelIt->geometry.data];

    // Find the right category
    Category category = (node.type == Properties::SceneNode::Type::Model) ? Category::Normal : Category::Light;

    // Create Scene Node
    const auto& initAABB = model->localAABB;
    std::vector<std::string> materials;
    for (const auto& m : modelIt->materials)
        materials.push_back(m.data);
    scene->CreateNode(modelIt->geometry.data, materials, node.id.data, category, initAABB);

    // Set initial transformation
    SceneNode* sceneNode = scene->FindNodeByUuid(node.id.data);
    sceneNode->Move(node.transform.position);
    sceneNode->Scale(node.transform.scale);
    sceneNode->Rotate(RotationAxis::X, node.transform.rotation.x);
    sceneNode->Rotate(RotationAxis::Y, node.transform.rotation.y);
    sceneNode->Rotate(RotationAxis::Z, node.transform.rotation.z);

    // Load children
    for (const auto& c : node.children)
        LoadSceneNode(scene, c, models);
}

std::unique_ptr<Scene> SceneFactory::CreateScene(
    const Properties::Scene& scene,
    const std::vector<Properties::Model>& models)
{
    std::unique_ptr<Scene> sceneOut(std::make_unique<Scene>());

    for (const auto& n : scene.nodes)
        LoadSceneNode(sceneOut.get(), n, models);

    return sceneOut;
}
