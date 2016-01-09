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
        0.1f, 100.0f
    );

    // Create the GBuffer
    mGBuffer = std::make_unique<GBuffer>(mScreenWidth, mScreenHeight);

    // Initialize the TextRenderer
    mTextRenderer.Init(mScreenWidth, mScreenHeight);
    mTextRenderer.GetFontStore().LoadFont("visitor", "ext/visitor.ttf");

    // Initialize the AABBRenderer
    mAABBRenderer.Init();
    mAABBRenderer.SetProjection(mProjection);
    // Do not show AABBs by default
    mShowAABBs = false;

    // Initialize the ShadowRenderer
    mShadowRenderer.Init(1024, 1024);
    mShadowRenderer.SetModelStore(&mModelStore);

    // Set directional light position
    mDirLightPos = glm::vec3(-0.2f, -1.0f, -0.3f);
}

void Renderer::Update(float dt)
{
    (void) dt;

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
    // Render the shadow map
    {
        // Set the rendering scene
        mShadowRenderer.SetScene(mScene);

        // Set light's properties
        mShadowRenderer.SetLightPos(mDirLightPos);

        // Render depth map
        mShadowRenderer.Render(interpolation);
    }

    // Bind the GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer->Id());
    {
        // Clear the current framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Make the GeometryPass
        GeometryPass(interpolation);
    }
    // Unbind the GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear default framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Make the LightPass
    LightPass(interpolation);

    // Setup forward render context
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

        // Render sample text
        mTextRenderer.RenderText("ScaryBox Studios", 10, 10, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
    }
}

void Renderer::Shutdown()
{
    // Shutdown the ShadowRenderer
    mShadowRenderer.Shutdown();

    // Shutdown the AABBRenderer
    mAABBRenderer.Shutdown();

    // Shutdown TextRenderer
    mTextRenderer.Shutdown();

    // Destroy GBuffer
    mGBuffer.reset();

    // Explicitly deallocate GPU data
    mTextureStore.Clear();
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
            const glm::vec3& diffCol = mdl->material.diffuseColor;
            glUniform3f(glGetUniformLocation(progId, "material.diffuseColor"), diffCol.r, diffCol.g, diffCol.b);
            if (mdl->material.usesDiffTex)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mdl->material.diffuseTexId);
                glUniform1i(glGetUniformLocation(progId, "material.diffuseTexture"), 0);
            }

            // Specular
            glUniform1f(glGetUniformLocation(progId, "material.specularColor"), mdl->material.specularColor);
            if (mdl->material.usesSpecTex)
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mdl->material.specularTexId);
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
    {
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

        // Upload light relevant uniforms
        // Get the view matrix
        const auto& view = mView;

        // Setup lighting position parameters
        const glm::mat4 inverseView = glm::inverse(view);
        const glm::vec3 viewPos = glm::vec3(inverseView[3].x, inverseView[3].y, inverseView[3].z);
        GLint viewPosId = glGetUniformLocation(progId, "viewPos");
        glUniform3f(viewPosId, viewPos.x, viewPos.y, viewPos.z);

        // Set directional light properties
        glUniform3f(glGetUniformLocation(progId, "dirLight.direction"), mDirLightPos.x, mDirLightPos.y, mDirLightPos.z);
        glUniform3f(glGetUniformLocation(progId, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(progId, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(progId, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

        // Set light's properties
        auto& categories = mScene->GetCategories();
        auto lightIt = categories.find(SceneNodeCategory::Light);
        const glm::mat4& lTrans = lightIt->second[0]->GetTransformation().GetInterpolated(interpolation);
        const glm::vec3 lightPos = glm::vec3(lTrans[3].x, lTrans[3].y, lTrans[3].z);
        glUniform3f(glGetUniformLocation(progId, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(progId, "light.ambient"),  0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(progId, "light.diffuse"),  0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(progId, "light.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(progId, "light.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(progId, "light.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(progId, "light.quadratic"), 0.032f);

        // Set material properties
        glUniform1f(glGetUniformLocation(progId, "shininess"), 32.0f);

        // Calculate light space matrix
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
        glm::mat4 lightView = glm::lookAt(mDirLightPos, glm::vec3(0.0f), glm::vec3(1.0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        glUniformMatrix4fv(glGetUniformLocation(progId, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Render final contents
        RenderQuad();
    }
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

TextureStore& Renderer::GetTextureStore()
{
    return mTextureStore;
}

ModelStore& Renderer::GetModelStore()
{
    return mModelStore;
}

