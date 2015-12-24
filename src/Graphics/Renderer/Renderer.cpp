#include "Renderer.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
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

void Renderer::Init()
{
    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable face culling
    glEnable(GL_CULL_FACE);

    // Enable the depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Create the GBuffer
    mGBuffer = std::make_unique<GBuffer>(800, 600);
}

void Renderer::Update(float dt)
{
    (void) dt;

    // Update interpolation variables
    for (auto& p : mWorld)
        for (auto& gObj : p.second)
            gObj.transform.Update();
}

void Renderer::Render(float interpolation)
{
    GeometryPass(interpolation);
    LightPass(interpolation);
    CheckGLError();
}

void Renderer::Shutdown()
{
    // Destroy GBuffer
    mGBuffer.reset();

    // Explicitly deallocate GPU data
    mTextureStore.Clear();
    mModelStore.Clear();
    mShaderStore.Clear();
}

void Renderer::GeometryPass(float interpolation)
{
    // Bind the GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer->Id());

    // Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create the projection matrix
    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    // Get the view matrix
    const auto& view = mView;

    // Use the geometry pass program
    GLuint progId = mShaderStore["geometry_pass"];
    glUseProgram(progId);

    // Upload projection and view matrices
    auto projectionId = glGetUniformLocation(progId, "projection");
    glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(projection));
    auto viewId = glGetUniformLocation(progId, "view");
    glUniformMatrix4fv(viewId, 1, GL_FALSE, glm::value_ptr(view));

    // Iterate through world objects by category
    for (const auto& objCategory : mWorld)
    {
        for (const auto& gObj : objCategory.second)
        {
            // Calculate the model matrix
            glm::mat4 model = gObj.transform.GetInterpolated(interpolation);

            // Upload it
            auto modelId = glGetUniformLocation(progId, "model");
            glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(model));

            // Get the model
            ModelDescription* mdl = mModelStore[gObj.model];

            // Bind the needed textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mdl->diffTexId);
            GLuint diffuseId = glGetUniformLocation(progId, "texture_diffuse1");
            glUniform1i(diffuseId, 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mdl->specTexId);
            GLuint specId = glGetUniformLocation(progId, "texture_specular1");
            glUniform1i(specId, 1);

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

    // Unbind the GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::LightPass(float interpolation)
{
    // Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        const glm::mat4& lTrans = mLight->transform.GetInterpolated(interpolation);
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

void Renderer::RenderQuad()
{
    GLfloat quadVert[] =
    {
       -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
       -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    GLuint quadVao;
    glGenVertexArrays(1, &quadVao);
    glBindVertexArray(quadVao);
    {
        GLuint quadVbo;
        glGenBuffers(1, &quadVbo);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVert), &quadVert, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &quadVbo);
    }
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &quadVao);
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

auto Renderer::GetWorld() -> std::unordered_map<std::string, std::vector<WorldObject>>&
{
    return mWorld;
}
