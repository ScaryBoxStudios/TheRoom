#include "SceneFactory.hpp"

#include <algorithm>
#include <assert.h>
#include "../Image/ImageLoader.hpp"
#include "../Geometry/ModelLoader.hpp"

SceneFactory::SceneFactory(TextureStore* tStore, ModelStore* mdlStore, MaterialStore* matStore, ScreenContext::FileDataCache* fdc)
    : mTextureStore(tStore)
    , mModelStore(mdlStore)
    , mMaterialStore(matStore)
    , mFileDataCache(fdc)
{
}

std::unique_ptr<Scene> SceneFactory::CreateFromSceneFile(const SceneFile& sceneFile)
{
    std::unique_ptr<Scene> scene(std::make_unique<Scene>());

    LoadTextures(sceneFile.textures, sceneFile.images);
    LoadMaterials(sceneFile.materials);
    LoadGeometries(sceneFile.geometries);
    LoadModels(sceneFile.object.children);

    // Add objects to scene
    for(auto& child : sceneFile.object.children)
    {
        // Find the right geometry
        ModelDescription* model = (*mModelStore)[child.geometry.ToString()];

        // Find the right category
        SceneNodeCategory category = (child.type.compare("Mesh") == 0) ? SceneNodeCategory::Normal : SceneNodeCategory::Light;

        const auto& initAABB = model->localAABB;
        scene->CreateNode(child.geometry.ToString(), child.material.ToString(), child.name, category, initAABB);

        scene->SetTransformation(child.name, child.matrix);
    }
    
    return std::move(scene);
}

void SceneFactory::LoadTextures(const std::vector<SceneFile::Texture>& textures, const std::vector<SceneFile::Image>& images)
{
    // The ImageLoader object
    ImageLoader imageLoader;

    for(auto& texture : textures)
    {
        // Ignore that texture if it has already been loaded
        if((*mTextureStore)[texture.uuid.ToString()] != nullptr)
            continue;

        // Find the coresponding image
        auto img = std::find_if(std::begin(images), std::end(images),
            [&texture](const SceneFile::Image& image)->bool
            {
                return texture.image == image.uuid;
            });

        // Assert if no image with the given UUID exists in scene file
        assert(img != std::end(images));

        std::string ext = img->url.substr(img->url.find_last_of(".") + 1);
        RawImage<> pb = imageLoader.Load(*(*mFileDataCache)[img->url], ext);
        mTextureStore->Load(texture.uuid.ToString(), pb);
    }
}

void SceneFactory::LoadMaterials(const std::vector<SceneFile::Material>& materials)
{
    for(auto& material : materials)
    {
        // Ignore that material if it has already been loaded
        if((*mMaterialStore)[material.uuid.ToString()] != nullptr)
            continue;

        Material newMat;

        const std::string& diffId      = material.map.ToString();
        const std::string& specId      = material.specMap.ToString();
        const std::string& normalMapId = material.nmap.ToString();

        if(diffId.compare("") != 0)
            newMat.SetDiffuseTexture((*mTextureStore)[diffId]->texId);

        // Add specular
        if(specId.compare("") != 0)
            newMat.SetSpecularTexture((*mTextureStore)[specId]->texId);

        // Add normal map
        if(normalMapId.compare("") != 0)
            newMat.SetNormalMapTexture((*mTextureStore)[normalMapId]->texId);

        // Add color
        newMat.SetDiffuseColor(glm::vec3(material.color.r, material.color.g, material.color.b));

        mMaterialStore->Load(material.uuid.ToString(), newMat);
    }
}

void SceneFactory::LoadGeometries(const std::vector<SceneFile::Geometry>& geometries)
{
    // Model loader instance
    ModelLoader modelLoader;

    for(auto& geometry : geometries)
    {
        // Ignore that geometry if it has alredy been loaded
        if((*mModelStore)[geometry.uuid.ToString()] != nullptr)
            continue;

        auto& file = (*mFileDataCache)[geometry.url];

        // Find file extension
        std::string ext = geometry.url.substr(geometry.url.find_last_of(".") + 1);

        // Load model
        ModelData model = modelLoader.Load(*file, ext.c_str());
        if(model.meshes.size() == 0)
            throw std::runtime_error("Couldn't load model (" + geometry.url + ")");

        mModelStore->Load(geometry.uuid.ToString(), std::move(model));

        //(*mModelStore)[geometry.uuid.ToString()]->material = *(*mMaterialStore)[m.material];
    }
}

void SceneFactory::LoadModels(const std::vector<SceneFile::Object::Child>& children)
{
    for(auto& child : children)
    {
        // Find the right geometry
        ModelDescription* model = (*mModelStore)[child.geometry.ToString()];

        // Assert if no geometry with the given name exists
        assert(model != nullptr);
    }
}