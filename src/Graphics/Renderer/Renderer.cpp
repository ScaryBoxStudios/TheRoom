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

void Renderer::Init(int width, int height, GLuint gPassProgId, GLuint lPassProgId, MaterialStore* materialStore)
{
    // Skybox is initially unset
    mSkybox = nullptr;

    // Scenen is initially unset
    mScene = nullptr;

    // Store the needed program id's
    mGeometryPassProgId = gPassProgId;
    mLightingPassProgId = lPassProgId;

    // Store the screen size
    mScreenWidth = width;
    mScreenHeight = height;

    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable the depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Create the projection matrix
    mProjection = glm::perspective(
        45.0f,
        static_cast<float>(mScreenWidth) / mScreenHeight,
        0.1f, 300.0f
    );

    // Create the GBuffer
    mGBuffer = std::make_unique<GBuffer>(mScreenWidth, mScreenHeight);

    // Initialize the null program used by stencil passes
    Shader vert(nullVShader, Shader::Type::Vertex);
    Shader frag(nullFShader, Shader::Type::Fragment);
    mNullProgram = std::make_unique<ShaderProgram>(vert.Id(), frag.Id());

    // Initialize the AABBRenderer
    mAABBRenderer.Init();
    mAABBRenderer.SetProjection(mProjection);
    // Do not show AABBs by default
    mShowAABBs = false;

    // Initialize the ShadowRenderer
    mShadowRenderer.Init(8096, 8096);
    mShadowRenderer.SetModelStore(&mModelStore);

    // Set the material store
    mMaterialStore = materialStore;

    // Add main directional light
    DirLight dirLight;
    dirLight.direction = glm::vec3(-0.3f, -0.5f, -0.5f);
    dirLight.properties.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    dirLight.properties.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    dirLight.properties.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    mLights.dirLights.push_back(dirLight);

    // Add point light
    PointLight pointLight;
    pointLight.properties.ambient  = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight.properties.diffuse  = glm::vec3(0.5f, 0.5f, 0.5f);
    pointLight.properties.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight.attProps.constant   = 1.0f;
    pointLight.attProps.linear     = 0.09f;
    pointLight.attProps.quadratic  = 0.032f;
    mLights.pointLights.push_back(pointLight);
}

void Renderer::Update(float dt)
{
    (void) dt;
    if (!mScene)
        return;

    // Update interpolation variables
    for (auto& obj : mScene->GetNodes())
    {
        Transform& trans = obj.second->GetTransformation();
        trans.Update();

        AABB& aabb = obj.second->GetAABB();
        aabb.Update(trans.GetPosition(), trans.GetScale(), trans.GetRotation());
    }
}

void Renderer::Render(float interpolation)
{
    // Setup clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //
    // Render the shadow map
    //
    // Set the rendering scene
    mShadowRenderer.SetScene(mScene);
    // Set light's properties
    mShadowRenderer.SetLightPos(-(mLights.dirLights.front().direction));
    // Render depth map
    mShadowRenderer.Render(interpolation);

    //
    // Make the GeometryPass
    //
    GeometryPass(interpolation);

    //
    // Make the LightPass
    //
    LightPass(interpolation);

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
        // Render the skybox
        if (mSkybox)
            mSkybox->Render(mProjection, mView);

        // Render the AABBs if enabled
        if (mShowAABBs)
            mAABBRenderer.Render(interpolation);
    }
    glDisable(GL_DEPTH_TEST);
}

void Renderer::Shutdown()
{
    // Shutdown the ShadowRenderer
    mShadowRenderer.Shutdown();

    // Shutdown the AABBRenderer
    mAABBRenderer.Shutdown();

    // Destroy GBuffer
    mGBuffer.reset();

    // Explicitly deallocate GPU data
    mModelStore.Clear();
}

void Renderer::GeometryPass(float interpolation)
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

    // Get the view matrix
    const auto& view = mView;

    // Use the geometry pass program
    GLuint progId = mGeometryPassProgId;
    glUseProgram(progId);

    // Upload projection and view matrices
    auto projectionId = glGetUniformLocation(progId, "projection");
    glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(mProjection));
    auto viewId = glGetUniformLocation(progId, "view");
    glUniformMatrix4fv(viewId, 1, GL_FALSE, glm::value_ptr(view));

    for(auto& p : mScene->GetNodes())
    {
        SceneNode* node = p.second.get();

        // Calculate the model matrix
        glm::mat4 model = node->GetTransformation().GetInterpolated(interpolation);

        // Upload it
        auto modelId = glGetUniformLocation(progId, "model");
        glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(model));

        // Get the model
        ModelDescription* mdl = mModelStore[node->GetModel()];

        // Get the material
        Material* material = (*mMaterialStore)[node->GetMaterial()];

        //
        // Upload material parameters
        //
        // Diffuse
        const glm::vec3& diffCol = material->GetDiffuseColor();
        glUniform3f(glGetUniformLocation(progId, "material.diffuseColor"), diffCol.r, diffCol.g, diffCol.b);
        if(material->UsesDiffuseTexture())
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material->GetDiffuseTexture());
            glUniform1i(glGetUniformLocation(progId, "material.diffuseTexture"), 0);
        }

        // Specular
        glUniform1f(glGetUniformLocation(progId, "material.specularColor"), material->GetSpecularColor().x);
        if(material->UsesSpecularTexture())
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material->GetSpecularTexture());
            glUniform1i(glGetUniformLocation(progId, "material.specularTexture"), 1);
        }

        // Normal map
        if(material->UsesNormalMapTexture())
        {
            glUniform1i(glGetUniformLocation(progId, "useNormalMaps"), GL_TRUE);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, material->GetNormalMapTexture());
            glUniform1i(glGetUniformLocation(progId, "normalMap"), 2);
        }
        else
        {
            glUniform1i(glGetUniformLocation(progId, "useNormalMaps"), GL_FALSE);
        }

        // Draw all its meshes
        for(const auto& mesh : mdl->meshes)
        {
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
    float MaxChannel = fmax(fmax(light.properties.diffuse.r, light.properties.diffuse.g), light.properties.diffuse.b);
    float ret =
        (-light.attProps.linear
         +sqrtf(light.attProps.linear * light.attProps.linear - 4 * light.attProps.quadratic
                * (light.attProps.constant - (256.0f / 5.0f) * MaxChannel))) / (2 * light.attProps.quadratic);
    return ret;
}

void Renderer::LightPass(float interpolation)
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

    // Bind the data textures
    GLuint gPosId = glGetUniformLocation(progId, "gPosition");
    glUniform1i(gPosId, 0);
    GLuint gNormId = glGetUniformLocation(progId, "gNormal");
    glUniform1i(gNormId, 1);
    GLuint gAlbSpecId = glGetUniformLocation(progId, "gAlbedoSpec");
    glUniform1i(gAlbSpecId, 2);

    // Bind the shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mShadowRenderer.DepthMapId());
    glUniform1i(glGetUniformLocation(progId, "shadowMap"), 3);

    // Pass the screen size
    glUniform2i(glGetUniformLocation(progId, "gScreenSize"), mScreenWidth, mScreenHeight);

    // Set material properties
    glUniform1f(glGetUniformLocation(progId, "shininess"), 32.0f);

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
    glUniform3fv(glGetUniformLocation(progId, "dirLight.direction"),           1, glm::value_ptr(dirLight.direction));
    glUniform3fv(glGetUniformLocation(progId, "dirLight.properties.ambient"),  1, glm::value_ptr(dirLight.properties.ambient));
    glUniform3fv(glGetUniformLocation(progId, "dirLight.properties.diffuse"),  1, glm::value_ptr(dirLight.properties.diffuse));
    glUniform3fv(glGetUniformLocation(progId, "dirLight.properties.specular"), 1, glm::value_ptr(dirLight.properties.specular));

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
    PointLight& pLight = mLights.pointLights.front();

    // Make the stencil pass
    StencilPass(pLight);
    mGBuffer->PrepareFor(GBuffer::Mode::LightPass);
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer->Id());

    //
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glUniform3fv(glGetUniformLocation(progId, "pLight.position"),            1, glm::value_ptr(pLight.position));
    glUniform3fv(glGetUniformLocation(progId, "pLight.properties.ambient"),  1, glm::value_ptr(pLight.properties.ambient));
    glUniform3fv(glGetUniformLocation(progId, "pLight.properties.diffuse"),  1, glm::value_ptr(pLight.properties.diffuse));
    glUniform3fv(glGetUniformLocation(progId, "pLight.properties.specular"), 1, glm::value_ptr(pLight.properties.specular));
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

void Renderer::SetSkybox(const Skybox* skybox)
{
    mSkybox = skybox;
}

void Renderer::SetScene(const Scene* scene)
{
    mScene = scene;
    mAABBRenderer.SetScene(scene);
}

void Renderer::SetView(const glm::mat4& view)
{
    mView = view;
    mAABBRenderer.SetView(view);
}

void Renderer::ToggleShowAABBs()
{
    mShowAABBs = !mShowAABBs;
}

ModelStore& Renderer::GetModelStore()
{
    return mModelStore;
}

void Renderer::SetPointLightPos(unsigned int index, const glm::vec3& pos)
{
    auto& pointLights = mLights.pointLights;

    // Do nothing if index is invalid
    if(pointLights.size() <= index)
        return;

    pointLights[index].position = pos;
}
