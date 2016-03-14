#include "SceneFactory.hpp"

#include <algorithm>
#include <assert.h>
#include "../Image/ImageLoader.hpp"
#include "../Geometry/ModelLoader.hpp"
#include "../../Util/FileLoad.hpp"

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

    BakeScene(scene.get(), sceneFile.object.children);

    return std::move(scene);
}

void SceneFactory::LoadTextures(const std::vector<SceneFile::Texture>& textures, const std::vector<SceneFile::Image>& images)
{
    // The ImageLoader object
    ImageLoader imageLoader;

    for(auto& texture : textures)
    {
        // Ignore that texture if it has already been loaded
        if((*mTextureStore)[std::to_string(texture.id)] != nullptr)
            continue;

        // Find the coresponding image
        auto img = std::find_if(std::begin(images), std::end(images),
            [&texture](const SceneFile::Image& image)->bool
            {
                return texture.image == image.id;
            }
        );

        // Assert if no image with the given UUID exists in scene file
        assert(img != std::end(images));

        std::string ext = img->url.substr(img->url.find_last_of(".") + 1);
        RawImage<> pb = imageLoader.Load(*(*mFileDataCache)[img->url], ext);
        mTextureStore->Load(std::to_string(texture.id), pb);
    }
}

void SceneFactory::LoadMaterials(const std::vector<SceneFile::Material>& materials)
{
    for(auto& material : materials)
    {
        // Ignore that material if it has already been loaded
        if((*mMaterialStore)[std::to_string(material.id)] != nullptr)
            continue;

        Material newMat;

        const std::string& diffId      = std::to_string(material.map);
        const std::string& specId      = std::to_string(material.specMap);
        const std::string& normalMapId = std::to_string(material.nmap);

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
        newMat.SetSpecularColor(glm::vec3(material.specular.r, material.specular.g, material.specular.b));
        newMat.SetRoughness(material.roughness);
        newMat.SetFresnel(material.fresnel);

        mMaterialStore->Load(std::to_string(material.id), newMat);
    }
}

void SceneFactory::LoadGeometries(const std::vector<SceneFile::Geometry>& geometries)
{
    // Model loader instance
    ModelLoader modelLoader;

    for(auto& geometry : geometries)
    {
        // Ignore that geometry if it has alredy been loaded
        if((*mModelStore)[std::to_string(geometry.id)] != nullptr)
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

        mModelStore->Load(std::to_string(geometry.id), std::move(model));

        //(*mModelStore)[geometry.uuid.ToString()]->material = *(*mMaterialStore)[m.material];
    }
}

void SceneFactory::BakeScene(Scene* const sceneToBake, const std::vector<SceneFile::Object::Child>& children)
{
    // Add objects to scene
    for(auto& child : children)
    {
        // Find the right geometry
        ModelDescription* model = (*mModelStore)[std::to_string(child.geometry)];

        // Find the right category
        Category category = (child.type.compare("Mesh") == 0) ? Category::Normal : Category::Light;

        // Create Scene Node
        const auto& initAABB = model->localAABB;
        std::vector<std::string> materials;
        for (const auto& mat : child.materials)
            materials.push_back(std::to_string(mat));
        sceneToBake->CreateNode(std::to_string(child.geometry), materials, child.name, category, initAABB);

        // Set initial transformation
        SceneNode* node = sceneToBake->FindNodeByUuid(child.name);
        node->Move(child.transform.position);
        node->Scale(child.transform.scale);
        node->Rotate(RotationAxis::X, child.transform.rotation.x);
        node->Rotate(RotationAxis::Y, child.transform.rotation.y);
        node->Rotate(RotationAxis::Z, child.transform.rotation.z);
    }
}
