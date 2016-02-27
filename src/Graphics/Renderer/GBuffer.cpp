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

    // - Material index buffer
    glGenTextures(1, &mMatIndexBufId);
    glBindTexture(GL_TEXTURE_2D, mMatIndexBufId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mMatIndexBufId, 0);

    GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(4, attachments);

    // - Depth + stencil buffer
    glGenTextures(1, &mDepthStencilBufId);
    glBindTexture(GL_TEXTURE_2D, mDepthStencilBufId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilBufId, 0);

    // - Final buffer
    glGenTextures(1, &mFinalBufId);
    glBindTexture(GL_TEXTURE_2D, mFinalBufId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mFinalBufId, 0);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("OpenGL: Framebuffer incomplete!");

    // Unbind stuff
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GBuffer::~GBuffer()
{
    GLuint textures[6] = { mPositionBufId, mNormalBufId, mAlbedoSpecBufId, mMatIndexBufId, mDepthStencilBufId, mFinalBufId };
    glDeleteTextures(6, textures);
    glDeleteFramebuffers(1, &mGBufferId);
}

void GBuffer::PrepareFor(Mode mode)
{
    switch(mode)
    {
        case Mode::GeometryPass:
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mGBufferId);
            GLuint attachments[4] = {
                GL_COLOR_ATTACHMENT0
              , GL_COLOR_ATTACHMENT1
              , GL_COLOR_ATTACHMENT2
              , GL_COLOR_ATTACHMENT4
            };
            glDrawBuffers(4, attachments);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            break;
        }
        case Mode::LightPass:
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mGBufferId);
            glDrawBuffer(GL_COLOR_ATTACHMENT3);

            GLuint textureIds[] = {
                mPositionBufId,
                mNormalBufId,
                mAlbedoSpecBufId,
                mMatIndexBufId
            };

            for (unsigned int i = 0; i < 4; ++i)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, textureIds[i]);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            break;
        }
        case Mode::StencilPass:
        {
            glBindFramebuffer(GL_FRAMEBUFFER, mGBufferId);
            glDrawBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            break;
        }
    }
}

void GBuffer::CopyResultToDefault(int width, int height)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferId);
    glReadBuffer(GL_COLOR_ATTACHMENT3);
    glBlitFramebuffer(
        0, 0, width, height,
        0, 0, width, height,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

GLuint GBuffer::MatIndexId() const
{
    return mMatIndexBufId;
}
