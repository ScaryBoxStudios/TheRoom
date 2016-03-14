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
    if (mUBO != 0) {
        glDeleteBuffers(1, &mUBO);
        mUBO = 0;
    }
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
        float roughness;
        float fresnel;
        float padding[2];

        float diffCol[3];
        float padding2;

        float specCol[3];
        float padding3;
    };

    // Construct the data to be uploaded
    std::vector<MatData> matData;
    for (const auto& matDesc : mMaterialDescs)
    {
        MatData md = {};
        md.roughness = matDesc.material.GetRoughness();
        md.fresnel = matDesc.material.GetFresnel();
        md.diffCol[0] = matDesc.material.GetDiffuseColor().r / 255.0f;
        md.diffCol[1] = matDesc.material.GetDiffuseColor().g / 255.0f;
        md.diffCol[2] = matDesc.material.GetDiffuseColor().b / 255.0f;
        md.specCol[0] = matDesc.material.GetSpecularColor().r / 255.0f;
        md.specCol[1] = matDesc.material.GetSpecularColor().g / 255.0f;
        md.specCol[2] = matDesc.material.GetSpecularColor().b / 255.0f;
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
