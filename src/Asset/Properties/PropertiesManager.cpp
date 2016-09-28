#include "PropertiesManager.hpp"

#include <iostream>
#include <unordered_map>

#include "Properties.hpp"
#include "PropertiesLoader.hpp"
#include "PropertiesValidator.hpp"
#include "../../Util/FileLoad.hpp"

using BufferType = std::vector<std::uint8_t>;

// Util to load files
auto LoadFile(const std::string& filename)
{
    auto rVal = FileLoad<BufferType>(filename);
    if (!rVal)
        throw std::runtime_error("Couldn't load file (" + filename + ")");
    return rVal;
}

void PropertiesManager::Load(
     const std::vector<PropertiesManager::LoadInput>& scenes,
     const std::vector<PropertiesManager::LoadInput>& materials,
     const std::vector<PropertiesManager::LoadInput>& models)
{
    // Create containers
    std::vector<std::unique_ptr<BufferType>> filedata;
    PropertiesLoader::InputContainer sceneIn, materialIn, modelIn;

    // Load files
    {
        std::vector<PropertiesLoader::InputContainer*> out = {&sceneIn, &materialIn, &modelIn};
        size_t vecPos = 0;
        for (const auto& i : {scenes, materials, models})
        {
            for (const auto& j : i)
            {
                filedata.push_back(LoadFile(j.filename));
                out[vecPos]->insert({j.id, *filedata[filedata.size() - 1]});
            }

            ++vecPos;
        }
    }

    // Create the loader
    PropertiesLoader loader;

    // Output maps
    PropertiesLoader::OutputContainer<Properties::SceneFile> sceneOut;
    PropertiesLoader::OutputContainer<Properties::MaterialFile> materialOut;
    PropertiesLoader::OutputContainer<Properties::ModelFile> modelOut;

    // Load
    loader.LoadBulk(
        sceneIn,    sceneOut,
        materialIn, materialOut,
        modelIn,    modelOut);

    // Validation
    PropertiesValidator validator;
        PropertiesValidator::Result r = validator.ValidateBulk(materialOut, modelOut);

    std::cout << "Errors: " << std::endl;
    for (const auto& e : r.errors)
        std::cout << "\t" << e.id + ": " << PropertiesValidator::ErrorToString(e.code) << std::endl;
    std::cout << "Warnings: " << std::endl;
    for (const auto& w : r.warnings)
        std::cout << "\t" << w.id + ": " << PropertiesValidator::WarnToString(w.code) << std::endl;

}
