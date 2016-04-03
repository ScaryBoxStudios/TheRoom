#include "ShadowRenderer.hpp"
#include <assert.h>

WARN_GUARD_ON
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

static const char* vShader = R"foo(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(position, 1.0f);
}
)foo";

static const char* gShader = R"foo(
#version 330 core
#extension GL_ARB_gpu_shader5 : enable

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 uCascadesViewMatrices[4];
uniform mat4 uCascadesProjMatrices[4];

out float gLayer;
out vec3  vsPosition;

void main()
{
    for(int i = 0; i < gl_in.length(); ++i)
    {
        vec4 pos    = (uCascadesViewMatrices[gl_InvocationID] * gl_in[i].gl_Position);
        gl_Position = uCascadesProjMatrices[gl_InvocationID] * pos;
        vsPosition  = pos.xyz;
        gl_Layer    = gl_InvocationID;
        gLayer      = float(gl_InvocationID);
        EmitVertex();
    }
    EndPrimitive();
}
)foo";

static const char* fShader = R"foo(
#version 330 core

uniform float uCascadesNear[4];
uniform float uCascadesFar[4];

in float gLayer;
in vec3 vsPosition;

void main()
{
    int layer = int(gLayer);
    float linearDepth = (-vsPosition.z - uCascadesNear[layer]) / (uCascadesFar[layer] - uCascadesNear[layer]);
    gl_FragDepth = linearDepth;
}
)foo";

void ShadowRenderer::Init(unsigned int width, unsigned int height)
{
    mWidth = width;
    mHeight = height;
    mSplitNum = 4;

    Shader vert(vShader, Shader::Type::Vertex);
    Shader geom(gShader, Shader::Type::Geometry);
    Shader frag(fShader, Shader::Type::Fragment);
    mProgram = std::make_unique<ShaderProgram>(vert.Id(), geom.Id(), frag.Id());

    // Create textures that will hold the shadow maps
    glGenTextures(1, &mDepthMapId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, mDepthMapId);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, width, height, mSplitNum, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    // Prepare framebuffer
    glGenFramebuffers(1, &mDepthMapFboId);
    glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFboId);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMapId, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthMapId, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check if framebuffer is complete
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void ShadowRenderer::Shutdown()
{
    glDeleteTextures(1, &mDepthMapId);
    glDeleteFramebuffers(1, &mDepthMapFboId);
}

void ShadowRenderer::Render(float interpolation, std::vector<IntMesh> scene)
{
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);

    // Store previous viewport and set the new one
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, mWidth, mHeight);

    // Bind shadow map fbo
    glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFboId);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram->Id());

        // Projection matrices
        glUniformMatrix4fv(
            glGetUniformLocation(mProgram->Id(), "uCascadesProjMatrices"),
            static_cast<GLsizei>(mProjectionMats.size()), GL_FALSE,
            glm::value_ptr(mProjectionMats[0])
        );
        // View matrices
        glUniformMatrix4fv(
            glGetUniformLocation(mProgram->Id(), "uCascadesViewMatrices"),
            static_cast<GLsizei>(mViewMats.size()), GL_FALSE,
            glm::value_ptr(mViewMats[0])
        );
        // Near values
        glUniform1fv(
            glGetUniformLocation(mProgram->Id(), "uCascadesNear"),
            static_cast<GLsizei>(mNearPlanes.size()),
            mNearPlanes.data()
        );
        // Far values
        glUniform1fv(
            glGetUniformLocation(mProgram->Id(), "uCascadesFar"),
            static_cast<GLsizei>(mFarPlanes.size()),
            mFarPlanes.data()
        );

    for (const auto& gObj : scene)
    {
        // Upload needed uniforms
        glm::mat4 model = gObj.transform.GetInterpolated(interpolation);
        glUniformMatrix4fv(glGetUniformLocation(mProgram->Id(), "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(gObj.vaoId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gObj.eboId);
        glDrawElements(GL_TRIANGLES, gObj.numIndices, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Restore viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);
}

static float ExtractFovFromProjection(const glm::mat4& projection)
{
    // From the source code it is:
    // T const tanHalfFovy = tan(fovy / static_cast<T>(2));
    // Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
    float t = projection[1][1];
    float fov = glm::atan((1.0f / t)) * 2.0f;
    return fov;
}

static float ExtractAspectFromProjection(const glm::mat4& projection)
{
    // From the source code it is:
    // Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
    // Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
    float tanHalfFovy = 1.0f / projection[1][1];
    float t = projection[0][0];
    float aspect = 1.0f / (t * tanHalfFovy);
    return aspect;
}

void ShadowRenderer::SetLightViewParams(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& lightPos)
{
    // Get projection properties
    const float cameraFov = ExtractFovFromProjection(projection);
    const float cameraAspect = ExtractAspectFromProjection(projection);
    const float cameraNear = 0.01f;
    const float cameraFar = 300.0f;

    // Get view properties
    //const glm::mat4 inverseView = glm::inverse(view);
    //const glm::vec3 viewPos = glm::vec3(inverseView[3].x, inverseView[3].y, inverseView[3].z);
    //const glm::vec3 viewDir = glm::vec3(view * glm::vec4(0, 0, -1, 0));

    //
    mProjectionMats.clear();
    mViewMats.clear();
    mShadowMats.clear();
    mPlanes.clear();
    mNearPlanes.clear();
    mFarPlanes.clear();

    const float splitLambda = 0.5f;
    const unsigned int splitNum = mSplitNum;
    for(unsigned int i = 0; i < splitNum; ++i)
    {
        // Find the split planes using GPU Gem 3. Chap 10 "Practical Split Scheme".
        float splitNear = i > 0
            ? glm::mix(
                cameraNear + (static_cast<float>(i) / splitNum) * (cameraFar - cameraNear),
                cameraNear * powf(cameraFar / cameraNear, static_cast<float>(i) / splitNum),
                splitLambda)
            : cameraNear;
        float splitFar = i < splitNum - 1
            ? glm::mix(
                cameraNear + (static_cast<float>(i + 1) / splitNum) * (cameraFar - cameraNear),
                cameraNear * powf(cameraFar / cameraNear, static_cast<float>(i + 1) / splitNum),
                splitLambda)
            : cameraFar;

        // Create the projection for this split
        glm::mat4 splitProj = glm::perspective(cameraFov, cameraAspect, splitNear, splitFar);
        glm::mat4 inverseProjView = glm::inverse(splitProj * view);

        // Extract the frustum points
        glm::vec4 splitVertices[8] = {
            // Near plane
            {-1.f, -1.f, -1.f, 1.f},
            {-1.f,  1.f, -1.f, 1.f},
            { 1.f,  1.f, -1.f, 1.f},
            { 1.f, -1.f, -1.f, 1.f},
            // Far plane
            {-1.f, -1.f,  1.f, 1.f},
            {-1.f,  1.f,  1.f, 1.f},
            { 1.f,  1.f,  1.f, 1.f},
            { 1.f, -1.f,  1.f, 1.f}
        };
        for(unsigned int j = 0; j < 8; ++j)
        {
            splitVertices[j] = inverseProjView * splitVertices[j];
            splitVertices[j] /= splitVertices[j].w;
        }

        // Find the centroid in oder to construct a view matrix
        glm::vec4 splitCentroid(0.0f);
        for(unsigned int j = 0; j < 8; ++j)
            splitCentroid += splitVertices[j];
        splitCentroid /= 8.0f;

        // Construct the view matrix
        float dist = glm::max(splitFar - splitNear, glm::distance(splitVertices[5], splitVertices[6]));
        glm::mat4 viewMat = glm::lookAt(
            glm::vec3(splitCentroid) + lightPos * dist,
            glm::vec3(splitCentroid),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Transform split vertices to the light view space
        glm::vec4 splitVerticesLS[8];
        for(unsigned int j = 0; j < 8; ++j)
            splitVerticesLS[j] += viewMat * splitVertices[j];

        // Find the frustum bounding box in view space
        glm::vec4 min = splitVerticesLS[0];
        glm::vec4 max = splitVerticesLS[0];
        for(unsigned int j = 0; j < 8; ++j)
        {
            min = glm::min(min, splitVerticesLS[j]);
            max = glm::max(max, splitVerticesLS[j]);
        }

        // Create an orthogonal projection matrix with the corners
        float nearOffset = 10.0f;
        float farOffset = 20.0f;
        glm::mat4 projMat = glm::ortho(min.x, max.x, min.y, max.y, -max.z - nearOffset, -min.z + farOffset);

        // Save matrices and planes
        mProjectionMats.push_back(projMat);
        mViewMats.push_back(viewMat);
        mShadowMats.push_back(projMat * viewMat);
        mPlanes.push_back(glm::vec2(splitNear, splitFar));
        mNearPlanes.push_back(-max.z - nearOffset);
        mFarPlanes.push_back(-min.z + farOffset);
    }
}

GLuint ShadowRenderer::DepthMapId() const
{
    return mDepthMapId;
}

unsigned int ShadowRenderer::GetSplitNum() const
{
    return mSplitNum;
}

const std::vector<glm::mat4> ShadowRenderer::GetSplitProjMats() const
{
    return mProjectionMats;
}

const std::vector<glm::mat4> ShadowRenderer::GetSplitViewMats() const
{
    return mViewMats;
}

const std::vector<glm::mat4> ShadowRenderer::GetSplitShadowMats() const
{
    return mShadowMats;
}

const std::vector<glm::vec2> ShadowRenderer::GetSplitPlanes() const
{
    return mPlanes;
}

const std::vector<float> ShadowRenderer::GetSplitNearPlanes() const
{
    return mNearPlanes;
}

const std::vector<float> ShadowRenderer::GetSplitFarPlanes() const
{
    return mFarPlanes;
}
