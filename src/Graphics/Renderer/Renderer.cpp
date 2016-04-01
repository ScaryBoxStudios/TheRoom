#include "Renderer.hpp"
#include <GL/gl.h>
#include "RenderUtils.hpp"
#include "../Util/GLUtils.hpp"

WARN_GUARD_ON
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

static const char* nullVShader = R"foo(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0f);
}
)foo";

static const char* nullFShader = R"foo(
#version 330 core

void main()
{
}
)foo";

void Renderer::Init(int width, int height, GLuint gPassProgId, GLuint lPassProgId)
{
    // Store the needed program id's
    mGeometryPassProgId = gPassProgId;
    mLightingPassProgId = lPassProgId;

    // Initialize screen size dependent values
    Resize(width, height);

    // Initialize the null program used by stencil passes
    Shader vert(nullVShader, Shader::Type::Vertex);
    Shader frag(nullFShader, Shader::Type::Fragment);
    mNullProgram = std::make_unique<ShaderProgram>(vert.Id(), frag.Id());

    // Initialize the ShadowRenderer
    mShadowRenderer.Init(8096, 8096);

    // Get ubo index
    GLuint geometryPassUboIndex = glGetUniformBlockIndex(mGeometryPassProgId, "Matrices");
    // Link block to its binding point
    glUniformBlockBinding(mGeometryPassProgId, geometryPassUboIndex, 0);

    // Create UBO buffer
    glGenBuffers(1, &mUboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, mUboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Define the range of the buffer that links to a uniform binding point
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, mUboMatrices, 0, 2 * sizeof(glm::mat4));
}

void Renderer::Resize(int width, int height)
{
    mScreenWidth = width;
    mScreenHeight = height;
    mProjection = glm::perspective(
        45.0f,
        static_cast<float>(mScreenWidth) / mScreenHeight,
        0.1f, 300.0f
    );
    mGBuffer = std::make_unique<GBuffer>(width, height);
}

void Renderer::Update(float dt)
{
    (void) dt;
}

void Renderer::Render(float interpolation, const IntForm& intForm)
{
    // Setup clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //
    // Set view and projection matrices in UBO
    //
    glBindBuffer(GL_UNIFORM_BUFFER, mUboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mProjection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mView));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //
    // Render the shadow map
    //
    // Set the rendering scene
    std::vector<ShadowRenderer::IntMesh> shadowRendererIntForm;
    for (const auto& matEntry : intForm.materials)
        for (const auto& mesh : matEntry.second)
            shadowRendererIntForm.emplace_back(
                ShadowRenderer::IntMesh{mesh.transformation, mesh.vaoId, mesh.eboId, mesh.numIndices}
            );
    // Set light's properties
    mShadowRenderer.SetLightViewParams(mProjection, mView, -(mLights.dirLights.front().direction));
    // Render depth map
    mShadowRenderer.Render(interpolation, shadowRendererIntForm);

    //
    // Make the GeometryPass
    //
    GeometryPass(interpolation, intForm);

    //
    // Make the LightPass
    //
    LightPass(interpolation, intForm);

    //
    // Copy result to default fbo
    //
    mGBuffer->CopyResultToDefault(mScreenWidth, mScreenHeight);

    //
    // Setup forward render context
    //
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer->Id());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, mScreenWidth, mScreenHeight,
                      0, 0, mScreenWidth, mScreenHeight,
                      GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check for OpenGL errors
    CheckGLError();

    //
    // Forward render stuff on top
    //
    glEnable(GL_DEPTH_TEST);
    // Forward rendering block
    {
    }
}

void Renderer::Shutdown()
{
    // Shutdown the ShadowRenderer
    mShadowRenderer.Shutdown();

    // Destroy GBuffer
    mGBuffer.reset();
}

void Renderer::GeometryPass(float interpolation, const IntForm& intForm)
{
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);

    // Prepare and bind the GBuffer
    mGBuffer->PrepareFor(GBuffer::Mode::GeometryPass);
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer->Id());

    // Clear the current framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the geometry pass program
    GLuint progId = mGeometryPassProgId;
    glUseProgram(progId);

    // Set sampler locations
    glUniform1i(glGetUniformLocation(progId, "material.diffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(progId, "material.specularTexture"), 1);
    glUniform1i(glGetUniformLocation(progId, "normalMap"), 2);

    for (auto& p : intForm.materials)
    {
        auto& intMat    = p.first;
        auto& intMeshes = p.second;

        // Upload material's properties to GPU
        // Pass the material index in the material buffer object
        glUniform1ui(glGetUniformLocation(progId, "matIdx"), intMat.matIndex);

        //
        // Upload material parameters
        //
        // Diffuse
        GLuint diffTexId = intMat.diffTexId;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffTexId);

        // Specular
        GLuint specTexId = intMat.specTexId;
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specTexId);

        // Normal map
        if(intMat.useNormalMap)
        {
            glUniform1i(glGetUniformLocation(progId, "useNormalMaps"), GL_TRUE);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, intMat.nmapTexId);
        }
        else
            glUniform1i(glGetUniformLocation(progId, "useNormalMaps"), GL_FALSE);

        // Draw every mesh of this material
        for (const IntMesh& mesh : intMeshes)
        {
            // Upload model matrix
            auto modelId = glGetUniformLocation(progId, "model");
            glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(mesh.transformation.GetInterpolated(interpolation)));

            // Draw mesh
            glBindVertexArray(mesh.vaoId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eboId);
            glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }

    glUseProgram(0);

    // Unbind the GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Disable depth testing
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

float CalcPointLightBSphere(const PointLight& light)
{
    float MaxChannel = fmax(fmax(light.color.r, light.color.g), light.color.b);
    float ret =
        (-light.attProps.linear
         +sqrtf(light.attProps.linear * light.attProps.linear - 4 * light.attProps.quadratic
                * (light.attProps.constant - (256.0f / 5.0f) * MaxChannel))) / (2 * light.attProps.quadratic);
    return ret;
}

void Renderer::LightPass(float interpolation, const IntForm& intForm)
{
    (void)interpolation;

    // Prepare GBuffer for the light pass
    mGBuffer->PrepareFor(GBuffer::Mode::LightPass);
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer->Id());

    // Clear framebuffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Enable blending for multiple light passes
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    // Use the light pass program
    GLuint progId = mLightingPassProgId;
    glUseProgram(progId);

    // Setup material index buffer
    GLuint blockIndex = glGetUniformBlockIndex(progId, "MaterialDataBlock");
    GLuint bindingPointIndex = 1;
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointIndex, mMaterialStore->DataId());
    glUniformBlockBinding(progId, blockIndex, bindingPointIndex);

    // Bind the data textures
    GLuint gPosId = glGetUniformLocation(progId, "gPosition");
    glUniform1i(gPosId, 0);
    GLuint gNormId = glGetUniformLocation(progId, "gNormal");
    glUniform1i(gNormId, 1);
    GLuint gAlbSpecId = glGetUniformLocation(progId, "gAlbedoSpec");
    glUniform1i(gAlbSpecId, 2);
    GLuint gMatIdxId = glGetUniformLocation(progId, "gMatIdx");
    glUniform1i(gMatIdxId, 3);

    // Bind the shadow map
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, mShadowRenderer.DepthMapId());
    glUniform1i(glGetUniformLocation(progId, "shadowMap"), 4);

    // Bind the skybox cube
    GLuint envMapId = intForm.skyboxId;
    glActiveTexture(GL_TEXTURE5);
    glUniform1i(glGetUniformLocation(progId, "skybox"), 5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envMapId);

    // Bind the skysphere
    GLuint skysphereId = intForm.skysphereId;
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, skysphereId);
    glUniform1i(glGetUniformLocation(progId, "skysphere"), 6);

    // Pass the screen size
    glUniform2i(glGetUniformLocation(progId, "gScreenSize"), mScreenWidth, mScreenHeight);

    // Pass the light space matrix
    glUniformMatrix4fv(
        glGetUniformLocation(progId, "lightSpaceMatrix"),
        1, GL_FALSE,
        glm::value_ptr(mShadowRenderer.GetLightViewMatrix())
    );

    //
    // Directional light passes
    //
    // Setup lighting position parameters
    const glm::mat4 inverseView = glm::inverse(mView);
    const glm::vec3 viewPos = glm::vec3(inverseView[3].x, inverseView[3].y, inverseView[3].z);
    GLint viewPosId = glGetUniformLocation(progId, "viewPos");
    glUniform3f(viewPosId, viewPos.x, viewPos.y, viewPos.z);

    // Set directional light properties
    const DirLight& dirLight = mLights.dirLights.front();
    glUniform3fv(glGetUniformLocation(progId, "dirLight.direction"), 1, glm::value_ptr(dirLight.direction));
    glUniform3fv(glGetUniformLocation(progId, "dirLight.color"), 1, glm::value_ptr(dirLight.color));

    // Render
    glm::mat4 mvp = glm::mat4();
    glUniformMatrix4fv(glGetUniformLocation(progId, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(glGetUniformLocation(progId, "lMode"), 1);
    RenderQuad();

    //
    // Point light passes
    //
    // Enable stencil test for bounding sphere optimization
    glEnable(GL_STENCIL_TEST);

    // Set point light's properties
    for (const PointLight& pLight : mLights.pointLights)
    {
        // Make the stencil pass
        StencilPass(pLight);
        mGBuffer->PrepareFor(GBuffer::Mode::LightPass);
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer->Id());

        //
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glUniform3fv(glGetUniformLocation(progId, "pLight.position"), 1, glm::value_ptr(pLight.position));
        glUniform3fv(glGetUniformLocation(progId, "pLight.color"), 1, glm::value_ptr(pLight.color));
        glUniform1f(glGetUniformLocation( progId, "pLight.attProps.constant"),   pLight.attProps.constant);
        glUniform1f(glGetUniformLocation( progId, "pLight.attProps.linear"),     pLight.attProps.linear);
        glUniform1f(glGetUniformLocation( progId, "pLight.attProps.quadratic"),  pLight.attProps.quadratic);

        // Bounding sphere
        float scaleFactor = CalcPointLightBSphere(pLight);
        glm::mat4 bsMdl = glm::mat4();
        bsMdl = glm::translate(bsMdl, pLight.position);
        bsMdl = glm::scale(bsMdl, glm::vec3(scaleFactor));
        mvp = mProjection * mView * bsMdl;
        glUniformMatrix4fv(glGetUniformLocation(progId, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

        // Render
        glUniform1i(glGetUniformLocation(progId, "lMode"), 2);
        RenderSphere();

        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
    }

    // Disable and unbind things
    glDisable(GL_STENCIL_TEST);
    glUseProgram(0);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::StencilPass(const PointLight& pLight)
{
    // Prepare the GBuffer for the stencil pass
    mGBuffer->PrepareFor(GBuffer::Mode::StencilPass);
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer->Id());

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    // Stencil properties
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    // Calc bounding sphere model
    float scaleFactor = CalcPointLightBSphere(pLight);
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, pLight.position);
    model = glm::scale(model, glm::vec3(scaleFactor));

    // Store previously used program
    GLint prevProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);

    // Render sphere
    glUseProgram(mNullProgram->Id());
    glm::mat4 MVP = mProjection * mView * model;
    glUniformMatrix4fv(glGetUniformLocation(mNullProgram->Id(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
    RenderSphere();
    glUseProgram(prevProgram);

    // Disable and unbind stuff
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void Renderer::SetView(const glm::mat4& view)
{
    mView = view;
}

void Renderer::SetDataStores(MaterialStore* matStore)
{
    mMaterialStore = matStore;
}

Lights& Renderer::GetLights()
{
    return mLights;
}

const glm::mat4 Renderer::GetProjection() const
{
    return mProjection;
}

const std::vector<Renderer::TextureTarget> Renderer::GetTextureTargets() const
{
    return {
        TextureTarget{0, 3, mGBuffer->PosId()},
        TextureTarget{0, 3, mGBuffer->NormalId()},
        TextureTarget{0, 3, mGBuffer->AlbedoSpecId()},
        TextureTarget{0, 3, mGBuffer->FinalId()},
        TextureTarget{0, 2, mShadowRenderer.DepthMapId()}
    };
}
