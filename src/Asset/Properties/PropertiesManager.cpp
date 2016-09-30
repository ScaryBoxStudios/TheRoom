#include "PropertiesManager.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>

#include "Properties.hpp"
#include "PropertiesLoader.hpp"
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
void PropertiesManager::Load(
     const std::vector<PropertiesManager::LoadInput>& scenes,
     const std::vector<PropertiesManager::LoadInput>& materials,
     const std::vector<PropertiesManager::LoadInput>& models)
{
    // Create input containers
    PropertiesLoader::InputContainer sceneIn, materialIn, modelIn;
    std::vector<std::unique_ptr<BufferType>> filedata;

    // Load filedata
    LoadFiledata(
        {&scenes,  &materials,  &models},   // Input
        {&sceneIn, &materialIn, &modelIn},  // Input containers
        filedata);                          // Filedata buffer

    // Create the loader
    PropertiesLoader loader;

    // Output maps
    PropertiesLoader::OutputContainer<Properties::SceneFile> sceneOut;
    PropertiesLoader::OutputContainer<Properties::MaterialFile> materialOut;
    PropertiesLoader::OutputContainer<Properties::ModelFile> modelOut;

    // Parse properties
    loader.LoadBulk(
        sceneIn,    sceneOut,
        materialIn, materialOut,
        modelIn,    modelOut);

    // Validate properties
    PropertiesValidator validator;
    PropertiesValidator::Result r = validator.ValidateBulk(sceneOut, materialOut, modelOut);

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
