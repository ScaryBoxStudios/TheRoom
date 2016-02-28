#include "Cubemap.hpp"

Cubemap::Cubemap()
{
    glGenTextures(1, &mTextureId);
}

Cubemap::~Cubemap()
{
    glDeleteTextures(1, &mTextureId);
}

GLuint Cubemap::Id() const
{
    return mTextureId;
}

void Cubemap::SetData(const std::unordered_map<Target, RawImage<>>& images)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (const auto& p : images)
        glTexImage2D(static_cast<GLenum>(p.first), 0, GL_RGB,
                     p.second.GetProperties().width,
                     p.second.GetProperties().height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, p.second.Data());

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
