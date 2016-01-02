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
    mProgram = glCreateProgram();

    mVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(mVShader, 1, &vShader, 0);
    glCompileShader(mVShader);
    glAttachShader(mProgram, mVShader);

    mFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(mFShader, 1, &fShader, 0);
    glCompileShader(mFShader);
    glAttachShader(mProgram, mFShader);

    glLinkProgram(mProgram);

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

    glGenTextures(1, &mTextureId);
}

Skybox::~Skybox()
{
    glDeleteTextures(1, &mTextureId);

    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);

    glDeleteShader(mFShader);
    glDeleteShader(mVShader);
    glDeleteProgram(mProgram);
}

void Skybox::Load(const std::unordered_map<Target, RawImage<>>& images)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (const auto& p : images)
        glTexImage2D(static_cast<GLenum>(p.first), 0, GL_RGB,
                     p.second.GetProperties().width,
                     p.second.GetProperties().height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, p.second.Data());

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::Render(const glm::mat4& projection, const glm::mat4& view) const
{
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glUseProgram(mProgram);
    {
        // Remove any translation component of the view matrix
        glm::mat4 ntView = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(glGetUniformLocation(mProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(mProgram, "view"), 1, GL_FALSE, glm::value_ptr(ntView));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(mProgram, "skybox"), 0);

        glBindVertexArray(mVao);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindVertexArray(0);
    }
    glUseProgram(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
