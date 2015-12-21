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

    // Enable the depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
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
    // Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create the projection matrix
    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    // Get the view matrix
    const auto& view = mView;

    // Iterate through world objects by category
    for (const auto& objCategory : mWorld)
    {
        // The object material category iterating through
        const auto& type = objCategory.first;

        // Use the appropriate program
        GLuint progId = mShaderStore[type];
        glUseProgram(progId);

        if (type == "normal")
        {
            // Setup lighting position parameters
            const glm::mat4& lTrans = mLight->transform.GetInterpolated(interpolation);
            const glm::vec3 lightPos = glm::vec3(lTrans[3].x, lTrans[3].y, lTrans[3].z);
            GLint lightPosId = glGetUniformLocation(progId, "light.position");
            glUniform3f(lightPosId, lightPos.x, lightPos.y, lightPos.z);

            //const auto& viewPos = cameraPos;
            const glm::mat4 inverseView = glm::inverse(view);
            const glm::vec3 viewPos = glm::vec3(inverseView[3].x, inverseView[3].y, inverseView[3].z);
            GLint viewPosId = glGetUniformLocation(progId, "viewPos");
            glUniform3f(viewPosId, viewPos.x, viewPos.y, viewPos.z);

            // Set lights properties
            glUniform3f(glGetUniformLocation(progId, "light.ambient"),  0.2f, 0.2f, 0.2f);
            glUniform3f(glGetUniformLocation(progId, "light.diffuse"),  0.5f, 0.5f, 0.5f);
            glUniform3f(glGetUniformLocation(progId, "light.specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(progId, "light.constant"), 1.0f);
            glUniform1f(glGetUniformLocation(progId, "light.linear"), 0.09f);
            glUniform1f(glGetUniformLocation(progId, "light.quadratic"), 0.032f);

            // Set material properties
            glUniform1f(glGetUniformLocation(progId, "material.shininess"), 32.0f);
        }

        // Upload projection and view matrices
        auto projectionId = glGetUniformLocation(progId, "projection");
        glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(projection));
        auto viewId = glGetUniformLocation(progId, "view");
        glUniformMatrix4fv(viewId, 1, GL_FALSE, glm::value_ptr(view));

        // The actual object list
        const auto& list = objCategory.second;
        for (const auto& gObj : list)
        {
            // Calculate the model matrix
            glm::mat4 model = gObj.transform.GetInterpolated(interpolation);

            // Upload it
            auto modelId = glGetUniformLocation(progId, "model");
            glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(model));

            // Get the model
            ModelDescription* mdl = mModelStore[gObj.model];

            if (type == "normal")
            {
                // Bind the needed textures
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mdl->diffTexId);
                GLuint diffuseId = glGetUniformLocation(progId, "material.diffuse");
                glUniform1i(diffuseId, 0);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mdl->specTexId);
                GLuint specId = glGetUniformLocation(progId, "material.specular");
                glUniform1i(specId, 1);
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

        glUseProgram(0);
    }

    // Check for errors
    CheckGLError();
}

void Renderer::Shutdown()
{
    // Explicitly deallocate GPU data
    mTextureStore.Clear();
    mModelStore.Clear();
    mShaderStore.Clear();
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

