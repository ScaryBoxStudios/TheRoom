#include "Skysphere.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "../Geometry/ModelLoader.hpp"
#include "../Image/PixelBufferTraits.hpp"

static const char* ssVShader = R"foo(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvCoords;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = uvCoords;
}
)foo";

static const char* ssFShader = R"foo(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D skybox;

void main()
{
    color = texture(skybox, TexCoords);
}
)foo";

Skysphere::Skysphere()
{
    // Shader program
    Shader vert(ssVShader, Shader::Type::Vertex);
    Shader frag(ssFShader, Shader::Type::Fragment);
    mProgram = std::make_unique<ShaderProgram>(vert.Id(), frag.Id());

    // Sphere geometry
    ModelData sphere = GenUVSphere(1.0f, 32, 32);
    const auto& sphereData = sphere.meshes.front().data;
    const auto& sphereIndices = sphere.meshes.front().indices;
    mSphereNumIndices = static_cast<GLsizei>(sphereIndices.size());

    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER,
        sphereData.size() * sizeof(VertexData),
        sphereData.data(),
        GL_STATIC_DRAW
    );
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sphereIndices.size() * sizeof(GLuint),
        sphereIndices.data(),
        GL_STATIC_DRAW
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Vertex attributes
    glBindVertexArray(mVao);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)(offsetof(VertexData, vx)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)(offsetof(VertexData, tx)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

Skysphere::~Skysphere()
{
    glDeleteBuffers(1, &mEbo);
    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);
}

void Skysphere::Load(const RawImage<>& image)
{
    // Gen texture
    glGenTextures(1, &mTexture);

    // Setup image texture params
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Get upload parameters
    GLint format = image.GetProperties().channels == 4 ? GL_RGBA : GL_RGB;
    uint32_t width = image.GetProperties().width;
    uint32_t height = image.GetProperties().height;
    const GLvoid* data = image.Data();

    // Upload image data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Skysphere::Render(const glm::mat4& projection, const glm::mat4& view) const
{
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glUseProgram(mProgram->Id());
    {
        // Remove any translation component of the view matrix
        glm::mat4 ntView = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(glGetUniformLocation(mProgram->Id(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(mProgram->Id(), "view"), 1, GL_FALSE, glm::value_ptr(ntView));

        glUniform1i(glGetUniformLocation(mProgram->Id(), "skybox"), 0);

        // Render
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture);
        glBindVertexArray(mVao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
        glDrawElements(GL_TRIANGLES, mSphereNumIndices, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glUseProgram(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
