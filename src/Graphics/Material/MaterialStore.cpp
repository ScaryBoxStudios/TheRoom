#include "MaterialStore.hpp"

MaterialStore::MaterialStore()
{
}

MaterialStore::~MaterialStore()
{
    Clear();
}

void MaterialStore::Clear()
{
    mMaterials.clear();
}

void MaterialStore::Load(const std::string& name, const Material& material)
{
    mMaterials.insert({name, material});
}

Material* MaterialStore::operator[](const std::string& name)
{
    auto it = mMaterials.find(name);
    if(it == std::end(mMaterials))
        return nullptr;
    else
        return &(it->second);
}
