#include "Renderer.hpp"
#include <GL/gl.h>
#include "RenderUtils.hpp"
#include "../Util/GLUtils.hpp"

WARN_GUARD_ON
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF


void Renderer::Init(int width, int height, GLuint gPassProgId, GLuint lPassProgId)
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

    // Enable face culling
    glEnable(GL_CULL_FACE);

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

    // Initialize the AABBRenderer
    mAABBRenderer.Init();
    mAABBRenderer.SetProjection(mProjection);
    // Do not show AABBs by default
    mShowAABBs = false;

    // Initialize the ShadowRenderer
    mShadowRenderer.Init(8096, 8096);
    mShadowRenderer.SetModelStore(&mModelStore);

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

    // Render the shadow map
    {
        // Set the rendering scene
        mShadowRenderer.SetScene(mScene);

        // Set light's properties
        mShadowRenderer.SetLightPos(-(mLights.dirLights.front().direction));

        // Render depth map
        mShadowRenderer.Render(interpolation);
    }

    // Bind the GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer->Id());
    {
        // Clear the current framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Make the GeometryPass
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        GeometryPass(interpolation);
    }
    // Unbind the GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear default framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Make the LightPass
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_DEPTH_TEST);
    LightPass(interpolation);

    // Setup forward render context
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer->Id());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, mScreenWidth, mScreenHeight,
                      0, 0, mScreenWidth, mScreenHeight,
                      GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
    // Check for OpenGL errors
    CheckGLError();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Forward rendering block
    {
        // Render the skybox
        if (mSkybox)
            mSkybox->Render(mProjection, mView);

        // Render the AABBs if enabled
        if (mShowAABBs)
            mAABBRenderer.Render(interpolation);
    }
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

    // Iterate through world objects by category
    for (const auto& objCategory : mScene->GetCategories())
    {
        for (const auto& gObj : objCategory.second)
        {
            // Calculate the model matrix
            glm::mat4 model = gObj->GetTransformation().GetInterpolated(interpolation);

            // Upload it
            auto modelId = glGetUniformLocation(progId, "model");
            glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(model));

            // Get the model
            ModelDescription* mdl = mModelStore[gObj->GetModel()];

            //
            // Upload material parameters
            //
            // Diffuse
            const glm::vec3& diffCol = mdl->material.GetDiffuseColor();
            glUniform3f(glGetUniformLocation(progId, "material.diffuseColor"), diffCol.r, diffCol.g, diffCol.b);
            if (mdl->material.UsesDiffuseTexture())
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mdl->material.GetDiffuseTexture());
                glUniform1i(glGetUniformLocation(progId, "material.diffuseTexture"), 0);
            }

            // Specular
            glUniform1f(glGetUniformLocation(progId, "material.specularColor"), mdl->material.GetSpecularColor().x);
            if (mdl->material.UsesSpecularTexture())
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mdl->material.GetSpecularTexture());
                glUniform1i(glGetUniformLocation(progId, "material.specularTexture"), 1);
            }

            // Draw all its meshes
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
}

void Renderer::LightPass(float interpolation)
{
    // Use the light pass program
    GLuint progId = mLightingPassProgId;
    glUseProgram(progId);

    // Bind the data textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mGBuffer->PosId());
    GLuint gPosId = glGetUniformLocation(progId, "gPosition");
    glUniform1i(gPosId, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mGBuffer->NormalId());
    GLuint gNormId = glGetUniformLocation(progId, "gNormal");
    glUniform1i(gNormId, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mGBuffer->AlbedoSpecId());
    GLuint gAlbSpecId = glGetUniformLocation(progId, "gAlbedoSpec");
    glUniform1i(gAlbSpecId, 2);

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
    glUniform1i(glGetUniformLocation(progId, "lMode"), 1);
    RenderQuad();

    //
    // Point light passes
    //
    // Set point light's properties
    PointLight& pLight = mLights.pointLights.front();

    // Update changing position
    auto& categories = mScene->GetCategories();
    auto lightIt = categories.find(SceneNodeCategory::Light);
    const glm::mat4& lTrans = lightIt->second[0]->GetTransformation().GetInterpolated(interpolation);
    const glm::vec3 lightPos = glm::vec3(lTrans[3].x, lTrans[3].y, lTrans[3].z);
    pLight.position = lightPos;

    glUniform3fv(glGetUniformLocation(progId, "pLight.position"), 1, glm::value_ptr(pLight.position));
    glUniform3fv(glGetUniformLocation(progId, "pLight.properties.ambient"),  1, glm::value_ptr(pLight.properties.ambient));
    glUniform3fv(glGetUniformLocation(progId, "pLight.properties.diffuse"),  1, glm::value_ptr(pLight.properties.diffuse));
    glUniform3fv(glGetUniformLocation(progId, "pLight.properties.specular"), 1, glm::value_ptr(pLight.properties.specular));
    glUniform1f(glGetUniformLocation(progId, "pLight.attProps.constant"), pLight.attProps.constant);
    glUniform1f(glGetUniformLocation(progId, "pLight.attProps.linear"), pLight.attProps.linear);
    glUniform1f(glGetUniformLocation(progId, "pLight.attProps.quadratic"), pLight.attProps.quadratic);

    // Render
    glUniform1i(glGetUniformLocation(progId, "lMode"), 2);
    RenderQuad();

    glUseProgram(0);
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

