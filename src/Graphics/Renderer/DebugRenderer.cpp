#include "DebugRenderer.hpp"
#include <vector>
#include <tuple>
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
uniform sampler2DArray samplerArray;
uniform int samplerArrayIndex;

uniform int mode;

void main(void)
{
    vec2 UVCoords = (gl_FragCoord.xy - offset) / gScreenSize;
    if (mode == 0)
        color = texture(sampler, UVCoords);
    else if (mode == 1)
        color = vec4(vec3((texture(sampler, UVCoords)).r), 1.0);
    else if (mode == 2)
        color = vec4(vec3(texture(samplerArray, vec3(UVCoords, samplerArrayIndex)).r), 1.0);
    else
        color = vec4(1.0);
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

    // Count total textures
    unsigned int totalTextures = 0;
    for (unsigned int i = 0; i < mDbgTextures.size(); ++i)
        totalTextures += std::get<0>(mDbgTextures[i]);

    // Mini screen width and height
    float padding = 10;
    float totalPadding = (totalTextures + 1) * padding;
    float scaleFactor = ((mWndWidth - totalPadding) / totalTextures) / mWndWidth;
    int curWidth = static_cast<int>(mWndWidth * scaleFactor);
    int curHeight = static_cast<int>(mWndHeight * scaleFactor);
    glUniform2i(glGetUniformLocation(mProgram->Id(), "gScreenSize"), curWidth, curHeight);

    unsigned int curTex = 0;
    for (std::size_t i = 0; i < mDbgTextures.size(); ++i)
    {
        // Get the texture count with given texture object (might be a texture array)
        std::int8_t numTextures = std::get<0>(mDbgTextures[i]);
        // Get texture channel number
        std::uint8_t channels = std::get<1>(mDbgTextures[i]);
        // Current rendering texture
        GLuint tex = std::get<2>(mDbgTextures[i]);

        int mode = -1;
        GLenum texType = GL_TEXTURE_2D;
        if (channels == 3 && numTextures == 1)
            mode = 0;
        else if (channels == 1 && numTextures == 1)
            mode = 1;
        else if (channels == 1 && numTextures > 1)
        {
            mode = 2;
            texType = GL_TEXTURE_2D_ARRAY;
        }

        // Set the shader mode
        glUniform1i(glGetUniformLocation(mProgram->Id(), "mode"), mode);

        for (int j = 0; j < numTextures; ++j)
        {
            // Setup sampler texture
            glActiveTexture(GL_TEXTURE0);
            const char* samplerUniformName = texType == GL_TEXTURE_2D ? "sampler" : "samplerArray";
            glUniform1i(glGetUniformLocation(mProgram->Id(), samplerUniformName), 0);
            glUniform1i(glGetUniformLocation(mProgram->Id(), "samplerArrayIndex"), j);

            // Set the viewport
            int xOffset = static_cast<int>(padding + (curWidth + padding) * curTex);
            int yOffset = static_cast<int>(padding);
            glViewport(xOffset, yOffset, curWidth, curHeight);
            glUniform2i(glGetUniformLocation(mProgram->Id(), "offset"), xOffset, yOffset);

            // Render
            glBindTexture(texType, tex);
            RenderQuad();
            glBindTexture(texType, 0);
            ++curTex;
        }
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

void DebugRenderer::SetDebugTextures(const std::vector<TextureTarget>& dbgTex)
{
    mDbgTextures = dbgTex;
}
