#include "ShadowRenderer.hpp"
#include <stdexcept>

WARN_GUARD_ON
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

static const char* vShader = R"foo(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
}
)foo";

static const char* fShader = R"foo(
#version 330 core

void main()
{
    // gl_FragDepth = gl_FragCoord.z;
}
)foo";

void ShadowRenderer::Init(unsigned int width, unsigned int height)
{
    mWidth = width;
    mHeight = height;

    Shader vert(vShader, Shader::Type::Vertex);
    Shader frag(fShader, Shader::Type::Fragment);
    mProgram = std::make_unique<ShaderProgram>(vert.Id(), frag.Id());

    glGenTextures(1, &mDepthMapId);

    glBindTexture(GL_TEXTURE_2D, mDepthMapId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &mDepthMapFboId);

    glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMapId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("OpenGL: Framebuffer incomplete!");
}

void ShadowRenderer::Shutdown()
{
    glDeleteTextures(1, &mDepthMapId);
    glDeleteFramebuffers(1, &mDepthMapFboId);
}

void ShadowRenderer::Render(float interpolation)
{
    glCullFace(GL_FRONT);

    // Store previous viewport and set the new one
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, mWidth, mHeight);

    // Bind shadow map fbo
    glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFboId);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(mProgram->Id());

        glUniformMatrix4fv(
            glGetUniformLocation(mProgram->Id(), "lightSpaceMatrix"), 1, GL_FALSE,
            glm::value_ptr(mLightViewMatrix)
        );

        for (const auto& gObj : mScene->GetNodes())
        {
            // Upload needed uniforms
            glm::mat4 model = gObj.second->GetTransformation().GetInterpolated(interpolation);
            glUniformMatrix4fv(glGetUniformLocation(mProgram->Id(), "model"), 1, GL_FALSE, glm::value_ptr(model));

            // Get the model
            ModelDescription* mdl = (*mModelStore)[gObj.second->GetModel()];
            if (mdl)
            {
                // Draw the model
                for (const auto& mesh : mdl->meshes)
                {
                    glBindVertexArray(mesh.vaoId);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eboId);
                    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }
        }

        glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Restore viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    glCullFace(GL_BACK);
}

void ShadowRenderer::SetScene(const Scene* scene)
{
    mScene = scene;
}

void ShadowRenderer::SetModelStore(ModelStore* modelStore)
{
    mModelStore = modelStore;
}

void ShadowRenderer::SetLightPos(const glm::vec3& lightPos)
{
    // Calculate light space matrix
    float orthoSz = 100.0f;
    float nearPlane = 1.0f;
    float farPlane = 75.0f;
    glm::mat4 lightProjection = glm::ortho(-orthoSz, orthoSz, -orthoSz, orthoSz, nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(1.0f));
    mLightViewMatrix = lightProjection * lightView;
}

GLuint ShadowRenderer::DepthMapId() const
{
    return mDepthMapId;
}

const glm::mat4& ShadowRenderer::GetLightViewMatrix() const
{
    return mLightViewMatrix;
}
