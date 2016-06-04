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

void Cubemap::SetData(const std::unordered_map<Target, RawImage<>>& images, GLuint level /*= 0*/)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (const auto& p : images)
        glTexImage2D(static_cast<GLenum>(p.first), level, GL_RGB,
                     p.second.GetProperties().width,
                     p.second.GetProperties().height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, p.second.Data());

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

static void crossFaceOffset(GLenum target, int* offx, int* offy, int width)
{
    int side = width / 4;
    switch (target)
    {
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            *offx = 2 * side;
            *offy = 1 * side;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            *offx = 0 * side;
            *offy = 1 * side;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            *offx = 1 * side;
            *offy = 0 * side;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            *offx = 1 * side;
            *offy = 2 * side;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            *offx = 1 * side;
            *offy = 1 * side;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            *offx = 3 * side;
            *offy = 1 * side;
            break;
    }
}

void Cubemap::SetData(const RawImage<>& img, GLuint level /*= 0*/)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Calc image params
    int stride = img.GetProperties().width;
    int width = img.GetProperties().width / 4;
    int height = img.GetProperties().height / 3;
    int channels = img.GetProperties().channels;

    // Set row read stride
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);

    // Upload image data
    for (int i = 0; i < 6; ++i)
    {
        int xoff, yoff;
        int target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        crossFaceOffset(target, &xoff, &yoff, stride);
        glTexImage2D(
            target, level, channels == 3 ? GL_RGB : GL_RGBA,
            width, height,
            0, channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,
            img.Data() + (yoff * stride + xoff) * channels);
    }

    // Reset row stride
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
