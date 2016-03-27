#include "DebugRenderer.hpp"
#include <vector>
#include "RenderUtils.hpp"

WARN_GUARD_ON
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

const GLchar* vertShSrc = R"foo(
#version 330
layout(location=0) in vec3 position;

void main(void)
{
    gl_Position = vec4(position, 1.0f);
}
)foo";

const GLchar* fragShSrc = R"foo(
#version 330
out vec4 color;

uniform ivec2 gScreenSize;
uniform ivec2 offset;
uniform sampler2D sampler;

void main(void)
{
    vec2 UVCoords = (gl_FragCoord.xy - offset) / gScreenSize;
    color = texture(sampler, UVCoords);
}
)foo";

void DebugRenderer::Init(int width, int height)
{
    SetWindowDimensions(width, height);

    Shader vShader(vertShSrc, Shader::Type::Vertex);
    Shader fShader(fragShSrc, Shader::Type::Fragment);
    mProgram = std::make_unique<ShaderProgram>(vShader.Id(), fShader.Id());
}

void DebugRenderer::Render(float interpolation)
{
    (void) interpolation;

    // Store previous viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Store previous depth test state
    GLint depthTest;
    glGetIntegerv(GL_DEPTH_TEST, &depthTest);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(mProgram->Id());
    // Setup sampler texture
    glUniform1i(glGetUniformLocation(mProgram->Id(), "sampler"), 0);
    glActiveTexture(GL_TEXTURE0);
    // Mini screen width and height
    float padding = 10;
    float totalPadding = (mDbgTextures.size() + 1) * padding;
    float scaleFactor = ((mWndWidth - totalPadding) / mDbgTextures.size()) / mWndWidth;
    int curWidth = mWndWidth * scaleFactor, curHeight = mWndHeight * scaleFactor;
    glUniform2i(glGetUniformLocation(mProgram->Id(), "gScreenSize"), curWidth, curHeight);

    for (std::size_t i = 0; i < mDbgTextures.size(); ++i)
    {
        // Current rendering texture
        GLuint tex = mDbgTextures[i];

        // Set the viewport
        int xOffset = padding + (curWidth + padding) * i, yOffset = padding;
        glViewport(xOffset, yOffset, curWidth, curHeight);
        glUniform2i(glGetUniformLocation(mProgram->Id(), "offset"), xOffset, yOffset);

        // Render
        glBindTexture(GL_TEXTURE_2D, tex);
        RenderQuad();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glUseProgram(0);

    // Restore previous depth test state
    if(depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    // Restore previous viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void DebugRenderer::Shutdown()
{
    mProgram.reset();
}

void DebugRenderer::SetWindowDimensions(int width, int height)
{
    mWndWidth =  width;
    mWndHeight = height;
}

void DebugRenderer::SetDebugTextures(const std::vector<GLuint>& dbgTex)
{
    mDbgTextures = dbgTex;
}
