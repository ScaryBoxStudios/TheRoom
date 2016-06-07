#include "Skybox.hpp"
#include <glm/gtc/type_ptr.hpp>

// The skybox cube vertex data
const GLfloat skyboxVertices[] = {
    // Positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

const char* vShader = R"foo(
#version 330 core
layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = position;
}
)foo";

const char* fShader = R"foo(
#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;

void main()
{
    color = texture(skybox, TexCoords);
}
)foo";

Skybox::Skybox()
{
    Shader vert(vShader, Shader::Type::Vertex);
    Shader frag(fShader, Shader::Type::Fragment);
    mProgram = std::make_unique<ShaderProgram>(vert.Id(), frag.Id());

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);
    {
        glGenBuffers(1, &mVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    mCubemap = std::make_unique<Cubemap>();
}

Skybox::~Skybox()
{
    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);
}

void Skybox::Load(const std::unordered_map<Cubemap::Target, RawImage<>>& images)
{
    mCubemap->SetData(images, 0);
}

void Skybox::Load(const RawImage<>& image)
{
    mCubemap->SetData(image, 0);
}

void Skybox::Render(const glm::mat4& projection, const glm::mat4& view) const
{
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glUseProgram(mProgram->Id());
    {
        // Remove any translation component of the view matrix
        glm::mat4 ntView = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(glGetUniformLocation(mProgram->Id(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(mProgram->Id(), "view"), 1, GL_FALSE, glm::value_ptr(ntView));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(mProgram->Id(), "skybox"), 0);

        glBindVertexArray(mVao);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemap->Id());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindVertexArray(0);
    }
    glUseProgram(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

const Cubemap* Skybox::GetCubemap() const
{
    return mCubemap.get();
}
