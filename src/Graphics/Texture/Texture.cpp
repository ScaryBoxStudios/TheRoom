#include "Texture.hpp"

Texture::Texture()
{
    glGenTextures(1, &mId);
}

Texture::~Texture()
{
    glDeleteTextures(1, &mId);
}

GLuint Texture::Id() const
{
    return mId;
}

