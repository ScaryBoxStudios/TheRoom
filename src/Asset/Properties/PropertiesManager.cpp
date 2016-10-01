#include "PropertiesManager.hpp"

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
    (void)mergedPropsBuffer;
    (void)scenes;
    (void)materials;
    (void)models;
    return *this;
}

void PropertiesManager::ValidateMergedProperties(Properties::SceneFile& mergedProps)
{
    (void)mergedProps;
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
