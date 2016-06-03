#include "TextureStore.hpp"

TextureStore::TextureStore()
{
}

TextureStore::~TextureStore()
{
    Clear();
}

void TextureStore::Clear()
{
    for (const auto& p : mTextures)
        glDeleteTextures(1, &p.second.texId);
    mTextures.clear();
}

TextureDescription* TextureStore::operator[](const std::string& name)
{
    auto it = mTextures.find(name);
    if (it == std::end(mTextures))
        return nullptr;
    else
        return &(it->second);
}

