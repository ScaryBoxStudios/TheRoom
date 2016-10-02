#include "PropertiesManager.hpp"

#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "PropertiesValidator.hpp"
#include "../../Util/FileLoad.hpp"

using BufferType = std::vector<std::uint8_t>;

// --------------------------------------------------
// Forward declarations for static functions
// --------------------------------------------------
// Util to load files
static std::unique_ptr<BufferType> LoadFile(const std::string& filename);

// Util to load file data to containers
static void LoadFiledata(
    std::vector<const std::vector<PropertiesManager::LoadInput>*> input,
    std::vector<PropertiesLoader::InputContainer*> out,
    std::vector<std::unique_ptr<BufferType>>& dataBuf);

// Print validation results
static void PrintResult(const PropertiesValidator::Result& r);

// --------------------------------------------------
// Header function implementation
// --------------------------------------------------
Properties::SceneFile PropertiesManager::Load(
     const std::vector<PropertiesManager::LoadInput>& scenes,
     const std::vector<PropertiesManager::LoadInput>& materials,
     const std::vector<PropertiesManager::LoadInput>& models)
{
    // Output buffers
    SceneFileContainer sceneBuf;
    MaterialFileContainer materialBuf;
    ModelFileContainer modelBuf;

    // Merged properties buffer
    Properties::SceneFile merged = {};

    // Load Properties Pipeline
    ParseProperties(scenes, materials, models, sceneBuf, materialBuf, modelBuf)
    .ValidateProperties(sceneBuf, materialBuf, modelBuf)
    .MergeProperties(merged, sceneBuf, materialBuf, modelBuf)
    .ValidateMergedProperties(merged);

    return merged;
}

PropertiesManager& PropertiesManager::ParseProperties(
    const std::vector<LoadInput>& scenes,
    const std::vector<LoadInput>& materials,
    const std::vector<LoadInput>& models,
    SceneFileContainer& sceneBuf,
    MaterialFileContainer& materialBuf,
    ModelFileContainer& modelBuf)
{
    // Create containers
    std::vector<std::unique_ptr<BufferType>> filedata;
    PropertiesLoader::InputContainer sceneIn, materialIn, modelIn;

    // Load filedata
    LoadFiledata(
        {&scenes,  &materials,  &models},   // Input
        {&sceneIn, &materialIn, &modelIn},  // Input containers
        filedata);                          // Filedata buffer

    // Create the loader
    PropertiesLoader loader;

    // Parse properties
    loader.LoadBulk(
        sceneIn,    sceneBuf,
        materialIn, materialBuf,
        modelIn,    modelBuf);

    return *this;
}

PropertiesManager& PropertiesManager::ValidateProperties(
    const PropertiesManager::SceneFileContainer& scenes,
    const PropertiesManager::MaterialFileContainer& materials,
    const PropertiesManager::ModelFileContainer& models)
{
    // Validate properties
    PropertiesValidator validator;
    PropertiesValidator::Result r = validator.ValidateBulk(scenes, materials, models);

    // Print results
    PrintResult(r);

    return *this;
}

PropertiesManager& PropertiesManager::MergeProperties(
    Properties::SceneFile& mergedPropsBuffer,
    const SceneFileContainer& scenes,
    const MaterialFileContainer& materials,
    const ModelFileContainer& models)
{
    // Use std members for verbosity
    using std::move;
    using std::begin;
    using std::end;
    using std::back_inserter;
    using std::vector;

    // Create buffers to store properties in other files
    vector<Properties::Texture> texturesBuf;
    vector<Properties::Material> materialsBuf;
    vector<Properties::Geometry> geometriesBuf;
    vector<Properties::Model> modelsBuf;
    vector<Properties::SceneNode> sceneNodeBuf;

    auto addMaterialToBuf = [&texturesBuf, &materialsBuf](const Properties::MaterialFile& mf) -> void
    {
        texturesBuf.insert(end(texturesBuf), begin(mf.textures), end(mf.textures));
        materialsBuf.insert(end(materialsBuf), begin(mf.materials), end(mf.materials));
    };

    auto addModelToBuf = [&geometriesBuf, &modelsBuf](const Properties::ModelFile& mf) -> void
    {
        geometriesBuf.insert(end(geometriesBuf), begin(mf.geometries), end(mf.geometries));
        modelsBuf.insert(end(modelsBuf), begin(mf.models), end(mf.models));
    };

    // Scan material files
    for (const auto& p : materials)
        addMaterialToBuf(p.second);

    // Scan model files
    for (const auto& p : models)
        addModelToBuf(p.second);

    // Scan scene files
    for (const auto& p : scenes)
    {
        addMaterialToBuf(p.second.extraMaterials);
        addModelToBuf(p.second.extraModels);
        sceneNodeBuf.insert(
            end(sceneNodeBuf), begin(p.second.scene.nodes), end(p.second.scene.nodes));
    }

    // Merge everything into a single scene file
    Properties::MaterialFile& mergedMat = mergedPropsBuffer.extraMaterials;
    Properties::ModelFile& mergedMod = mergedPropsBuffer.extraModels;

    // Reserve space to move contents
    mergedMat.textures.reserve(texturesBuf.size() + mergedMat.textures.size());
    mergedMat.materials.reserve(materialsBuf.size() + mergedMat.materials.size());
    mergedMod.geometries.reserve(geometriesBuf.size() + mergedMod.geometries.size());
    mergedMod.models.reserve(modelsBuf.size() + mergedMod.models.size());
    mergedPropsBuffer.scene.nodes.reserve(sceneNodeBuf.size() + mergedPropsBuffer.scene.nodes.size());

    // Move contents
    move(begin(texturesBuf),   end(texturesBuf),   back_inserter(mergedMat.textures));
    move(begin(materialsBuf),  end(materialsBuf),  back_inserter(mergedMat.materials));
    move(begin(geometriesBuf), end(geometriesBuf), back_inserter(mergedMod.geometries));
    move(begin(modelsBuf),     end(modelsBuf),     back_inserter(mergedMod.models));
    move(begin(sceneNodeBuf),  end(sceneNodeBuf),  back_inserter(mergedPropsBuffer.scene.nodes));

    return *this;
}

void PropertiesManager::ValidateMergedProperties(Properties::SceneFile& mergedProps)
{
    // Validate properties
    PropertiesValidator validator;
    PropertiesValidator::Result r = validator.Validate(mergedProps);

    // Print results
    PrintResult(r);
}

// --------------------------------------------------
// Utilities
// --------------------------------------------------
static std::unique_ptr<BufferType> LoadFile(const std::string& filename)
{
    auto rVal = FileLoad<BufferType>(filename);
    if (!rVal)
        throw std::runtime_error("Couldn't load file (" + filename + ")");
    return rVal;
}

static void LoadFiledata(
    std::vector<const std::vector<PropertiesManager::LoadInput>*> input,
    std::vector<PropertiesLoader::InputContainer*> out,
    std::vector<std::unique_ptr<BufferType>>& dataBuf)
{
    size_t vecPos = 0;
    for (const auto& i : input)
    {
        for (const auto& j : *i)
        {
            dataBuf.push_back(LoadFile(j.filename));
            out[vecPos]->insert({j.id, *dataBuf[dataBuf.size() - 1]});
        }

        ++vecPos;
    }
}

static void PrintResult(const PropertiesValidator::Result& r)
{
    // Errors
    if (!r.errors.empty())
        std::cout << "Errors: " << std::endl;
    for (const auto& e : r.errors)
        std::cout << "\t" << e.id + ": " << PropertiesValidator::ErrorToString(e.code) << std::endl;

    // Warnings
    if (!r.warnings.empty())
        std::cout << "Warnings: " << std::endl;
    for (const auto& w : r.warnings)
        std::cout << "\t" << w.id + ": " << PropertiesValidator::WarnToString(w.code) << std::endl;

}
