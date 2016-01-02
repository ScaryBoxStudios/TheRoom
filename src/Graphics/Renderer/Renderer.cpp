#include "Renderer.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "RenderUtils.hpp"

WARN_GUARD_ON
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

///==============================================================
///= GL Helpers
///==============================================================
static void CheckGLError()
{
    GLenum errVal = glGetError();
    if (errVal != GL_NO_ERROR)
    {
        std::stringstream ss;
        ss << "OpenGL Error! Code: " << errVal;
        const char* desc = reinterpret_cast<const char*>(gluErrorString(errVal));
        throw std::runtime_error(std::string("OpenGL error: \n") + desc);
    }
}

void Renderer::Init(int width, int height)
{
    // Skybox is initially unset
    mSkybox = nullptr;

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
}

void Renderer::Update(float dt)
{
    (void) dt;

    // Update interpolation variables
    for (auto& obj : mScene.GetNodes())
        obj.second->GetTransformation().Update();
}

void Renderer::Render(float interpolation)
{
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

        // Render sample text
        mTextRenderer.RenderText("ScaryBox Studios", 10, 10, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
    }
}

void Renderer::Shutdown()
{
    // Shutdown TextRenderer
    mTextRenderer.Shutdown();

    // Destroy GBuffer
    mGBuffer.reset();

    // Explicitly deallocate GPU data
    mTextureStore.Clear();
    mModelStore.Clear();
    mShaderStore.Clear();
}

void Renderer::GeometryPass(float interpolation)
{
    // Get the view matrix
    const auto& view = mView;

    // Use the geometry pass program
    GLuint progId = mShaderStore["geometry_pass"];
    glUseProgram(progId);

    // Upload projection and view matrices
    auto projectionId = glGetUniformLocation(progId, "projection");
    glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(mProjection));
    auto viewId = glGetUniformLocation(progId, "view");
    glUniformMatrix4fv(viewId, 1, GL_FALSE, glm::value_ptr(view));

    // Iterate through world objects by category
    for (const auto& objCategory : mScene.GetCategories())
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
    GLuint progId = mShaderStore["light_pass"];
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

        // Upload light relevant uniforms
        // Get the view matrix
        const auto& view = mView;

        // Setup lighting position parameters
        const glm::mat4 inverseView = glm::inverse(view);
        const glm::vec3 viewPos = glm::vec3(inverseView[3].x, inverseView[3].y, inverseView[3].z);
        GLint viewPosId = glGetUniformLocation(progId, "viewPos");
        glUniform3f(viewPosId, viewPos.x, viewPos.y, viewPos.z);

        // Set light's properties
        auto& categories = mScene.GetCategories();
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

        // Render final contents
        RenderQuad();
    }
    glUseProgram(0);
}

void Renderer::SetSkybox(const Skybox* skybox)
{
    mSkybox = skybox;
}

void Renderer::SetView(const glm::mat4& view)
{
    mView = view;
}

TextureStore& Renderer::GetTextureStore()
{
    return mTextureStore;
}

ShaderStore& Renderer::GetShaderStore()
{
    return mShaderStore;
}

ModelStore& Renderer::GetModelStore()
{
    return mModelStore;
}

Scene& Renderer::GetScene()
{
    return mScene;
}
