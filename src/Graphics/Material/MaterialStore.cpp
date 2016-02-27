#include "MaterialStore.hpp"

MaterialStore::MaterialStore()
  : mUBO(0)
{
}

MaterialStore::~MaterialStore()
{
    Clear();
}

void MaterialStore::Clear()
{
    if (mUBO != 0)
        glDeleteBuffers(1, &mUBO);
    mMaterials.clear();
    mMaterialDescs.clear();
}

void MaterialStore::Load(const std::string& name, const Material& material)
{
    // Store material data
    mMaterialDescs.push_back({(GLuint)mMaterialDescs.size(), material});

    // Lazy initiate material ubo
    if (mUBO == 0)
        glGenBuffers(1, &mUBO);

    // The actual PACKED datatype that is uploaded to the UBO
    struct MatData
    {
        float shininess;
        float padding[3];
    };

    // Construct the data to be uploaded
    std::vector<MatData> matData;
    for (const auto& matDesc : mMaterialDescs)
    {
        MatData md = {};
        md.shininess = matDesc.material.GetShininess();
        matData.push_back(md);
    }

    // Reupload all materials to the GPU
    glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
    glBufferData(
        GL_UNIFORM_BUFFER,
        matData.size() * sizeof(MatData),
        matData.data(),
        GL_DYNAMIC_DRAW
    );
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Store material description to relational map
    mMaterials.insert({name, mMaterialDescs.size() - 1});
}

MaterialDescription* MaterialStore::operator[](const std::string& name)
{
    auto it = mMaterials.find(name);
    if(it == std::end(mMaterials))
        return nullptr;
    else
        return &mMaterialDescs[(it->second)];
}

GLuint MaterialStore::DataId() const
{
    return mUBO;
}
