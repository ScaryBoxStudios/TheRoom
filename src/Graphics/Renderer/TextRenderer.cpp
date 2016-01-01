#include "TextRenderer.hpp"

WARN_GUARD_ON
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

const char* textVertexSh = R"foo(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)foo";

const char* textFragSh = R"foo(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)foo";

void TextRenderer::Init(int width, int height)
{
    // Setup the ortho projection matrix
    mProjection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    // Setup the text rendering program
    mProgram = glCreateProgram();

    mVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(mVShader, 1, &textVertexSh, 0);
    glCompileShader(mVShader);
    glAttachShader(mProgram, mVShader);

    mFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(mFShader, 1, &textFragSh, 0);
    glCompileShader(mFShader);
    glAttachShader(mProgram, mFShader);

    glLinkProgram(mProgram);

    // Setup the rendering quad
    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);

    glBindVertexArray(mVao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);

        // 2D quad needs 6 vertices * 4 floats each (2 for pos + 2 for texCoords)
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
}

void TextRenderer::RenderText(const std::string& text, float x, float y, glm::vec3 color, const std::string& font)
{
    // Store previous blending state
    GLboolean blend = glIsEnabled(GL_BLEND);

    // Store previous src alpha blend function
    GLint srcAlphaBlendFunc;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlphaBlendFunc);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(mProgram);
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "projection"), 1, GL_FALSE, glm::value_ptr(mProjection));
    glUniform3f(glGetUniformLocation(mProgram, "textColor"), color.x, color.y, color.z);
    glUniform1i(glGetUniformLocation(mProgram, "text"), 0);

    glBindVertexArray(mVao);
    glActiveTexture(GL_TEXTURE0);

    float curX = x;
    // Iterate through all input characters
    for (const char c : text)
    {
        const Font* fontFace = mFontStore[font];
        const Font::Glyph* glyph = (*fontFace)[c];

        GLfloat xPos = curX + glyph->bearing.x;
        GLfloat yPos = y - (glyph->size.y - glyph->bearing.y);
        GLfloat width = static_cast<GLfloat>(glyph->size.x);
        GLfloat height = static_cast<GLfloat>(glyph->size.y);

        GLfloat vertices[6][4] =
        {
            { xPos, yPos + height, 0.0, 0.0 },
            { xPos, yPos, 0.0, 1.0 },
            { xPos + width, yPos, 1.0, 1.0 },
            { xPos, yPos + height, 0.0, 0.0 },
            { xPos + width, yPos, 1.0, 1.0 },
            { xPos + width, yPos + height, 1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, glyph->texId);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursor for next glyph (advance is number of 1/64 pixels)
        curX += (glyph->advance >> 6);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    // Restore blending options
    glBlendFunc(GL_SRC_ALPHA, srcAlphaBlendFunc);
    if(blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void TextRenderer::Shutdown()
{
    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);

    glDeleteShader(mFShader);
    glDeleteShader(mVShader);
    glDeleteProgram(mProgram);
}

FontStore& TextRenderer::GetFontStore()
{
    return mFontStore;
}
