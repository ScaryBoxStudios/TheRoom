#include "Texture.hpp"

Texture::Texture()
{
    glGenTextures(1, &mId);
}

Texture::~Texture()
{
    glDeleteTextures(1, &mId);
}

Texture::Texture(Texture&& other) :
    mId(other.mId)
{
    other.mId = 0;
}

Texture& Texture::operator=(Texture&& other)
{
    if (this != &other)
    {
        mId = other.mId;
        other.mId = 0;
    }
    return *this;
}

GLuint Texture::Id() const
{
    return mId;
}

