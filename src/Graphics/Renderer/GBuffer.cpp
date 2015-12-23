#include "GBuffer.hpp"
#include <stdexcept>

GBuffer::GBuffer(unsigned int width, unsigned int height)
{
    // Create the framebuffer
    glGenFramebuffers(1, &mGBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, mGBufferId);

    // - Position color buffer
    glGenTextures(1, &mPositionBufId);
    glBindTexture(GL_TEXTURE_2D, mPositionBufId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPositionBufId, 0);

    // - Normal color buffer
    glGenTextures(1, &mNormalBufId);
    glBindTexture(GL_TEXTURE_2D, mNormalBufId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormalBufId, 0);

    // - Color + Specular color buffer
    glGenTextures(1, &mAlbedoSpecBufId);
    glBindTexture(GL_TEXTURE_2D, mAlbedoSpecBufId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mAlbedoSpecBufId, 0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // - Create and attach depth buffer
    glGenRenderbuffers(1, &mDepthBufId);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBufId);

    // Unbind stuff
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("OpenGL: Framebuffer incomplete!");
}

GBuffer::~GBuffer()
{
    GLuint textures[3] = { mPositionBufId, mNormalBufId, mAlbedoSpecBufId };
    glDeleteRenderbuffers(1, &mDepthBufId);
    glDeleteTextures(3, textures);
    glDeleteFramebuffers(1, &mGBufferId);
}

GLuint GBuffer::Id() const
{
    return mGBufferId;
}

GLuint GBuffer::PosId() const
{
    return mPositionBufId;
}

GLuint GBuffer::NormalId() const
{
    return mNormalBufId;
}

GLuint GBuffer::AlbedoSpecId() const
{
    return mAlbedoSpecBufId;
}
