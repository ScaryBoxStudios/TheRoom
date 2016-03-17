#include "Material.hpp"

Material::Material()
  : mDiffColor(glm::vec3(0.0f))
  , mDiffTexture(0)
  , mSpecColor(glm::vec3(0.0f))
  , mSpecTexture(0)
  , mEmissiveColor(glm::vec3(0.0f))
  , mRoughness(0)
  , mFresnel(0)
  , mNMapTexture(0)
{
}

//
//=- Diffuse
//
const glm::vec3& Material::GetDiffuseColor() const
{
    return mDiffColor;
}

void Material::SetDiffuseColor(const glm::vec3& v)
{
    mDiffColor = v;
}

GLuint Material::GetDiffuseTexture() const
{
    return mDiffTexture;
}

void Material::SetDiffuseTexture(GLuint id)
{
    mDiffTexture = id;
}

bool Material::UsesDiffuseTexture() const
{
    return mDiffTexture != 0;
}

//
//=- Specular
//
const glm::vec3& Material::GetSpecularColor() const
{
    return mSpecColor;
}

void Material::SetSpecularColor(const glm::vec3& v)
{
    mSpecColor = v;
}

GLuint Material::GetSpecularTexture() const
{
    return mSpecTexture;
}

void Material::SetSpecularTexture(GLuint id)
{
    mSpecTexture = id;
}

bool Material::UsesSpecularTexture() const
{
    return mSpecTexture != 0;
}

float Material::GetRoughness() const
{
    return mRoughness;
}

void Material::SetRoughness(float s)
{
    mRoughness = s;
}

float Material::GetFresnel() const
{
    return mFresnel;
}

void Material::SetFresnel(float s)
{
    mFresnel = s;
}

//
//=- Specular
//
const glm::vec3& Material::GetEmissiveColor() const
{
    return mEmissiveColor;
}

void Material::SetEmissiveColor(const glm::vec3& col)
{
    mEmissiveColor = col;
}

//
//=- Normal Mapping
//
GLuint Material::GetNormalMapTexture() const
{
    return mNMapTexture;
}

void Material::SetNormalMapTexture(GLuint id)
{
    mNMapTexture = id;
}

bool Material::UsesNormalMapTexture() const
{
    return mNMapTexture != 0;
}
