#include "AABBRenderer.hpp"
#include "RenderUtils.hpp"

WARN_GUARD_ON
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

const GLchar* simpleVert = R"foo(
#version 330
layout(location=0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

void main(void)
{
    gl_Position = projection * view * vec4(position, 1.0f);
}
)foo";

const GLchar* simpleFrag = R"foo(
#version 330
out vec4 color;

void main(void)
{
    color = vec4(0.0f, 0.0f, 1.0f, 0.0f);
}
)foo";

void AABBRenderer::Init()
{
    Shader vShader(simpleVert, Shader::Type::Vertex);
    Shader fShader(simpleFrag, Shader::Type::Fragment);
    mProgram = std::make_unique<ShaderProgram>(vShader.Id(), fShader.Id());
}

void AABBRenderer::Render(float interpolation)
{
    (void) interpolation;

    glUseProgram(mProgram->Id());
    for(const auto& p : mScene->GetNodes())
    {
        SceneNode* const node = p.second.get();

        // Upload model, projection and view matrices
        glUniformMatrix4fv(glGetUniformLocation(mProgram->Id(), "projection"), 1, GL_FALSE, glm::value_ptr(mProjection));
        glUniformMatrix4fv(glGetUniformLocation(mProgram->Id(), "view"), 1, GL_FALSE, glm::value_ptr(mView));

        // Draw the AABB
        RenderBox(node->GetAABB());
    }
    glUseProgram(0);
}

void AABBRenderer::Shutdown()
{
    mScene = nullptr;
    mProgram.reset();
}

void AABBRenderer::SetScene(const Scene* scene)
{
    mScene = scene;
}

void AABBRenderer::SetProjection(const glm::mat4& projection)
{
    mProjection = projection;
}

void AABBRenderer::SetView(const glm::mat4& view)
{
    mView = view;
}
