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

void TextureStore::Load(const std::string& name, const RawImage& img)
{
    // Gen texture
    GLuint id;
    glGenTextures(1, &id);

    // Load data
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint format = img.Channels() == 4 ? GL_RGBA : GL_RGB;
    uint32_t width = img.Width();
    uint32_t height = img.Height();
    const GLvoid* data = img.Data();

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Store
    TextureDescription td;
    td.texId = id;
    mTextures.insert({name, td});
}


TextureDescription* TextureStore::operator[](const std::string& name)
{
    auto it = mTextures.find(name);
    if (it == std::end(mTextures))
        return nullptr;
    else
        return &(it->second);
}

