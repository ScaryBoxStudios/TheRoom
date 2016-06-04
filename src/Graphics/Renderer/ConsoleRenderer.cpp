#include "ConsoleRenderer.hpp"
#include <glad/glad.h>
#include "RenderUtils.hpp"
#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

static const GLchar* vertShSrc = R"foo(
#version 330
layout(location=0) in vec2 position;

void main(void)
{
    gl_Position = vec4(position, 1.0f, 1.0f);
}
)foo";

static const GLchar* fragShSrc = R"foo(
#version 330
out vec4 color;

void main(void)
{
    color = vec4(0.0, 0.0, 0.0, 0.3);
}
)foo";
void ConsoleRenderer::Init(TextRenderer* textRenderer)
{
    mTextRenderer = textRenderer;

    // Load default font
    mTextRenderer->GetFontStore().LoadFont("atari", "ext/Assets/Fonts/atari.ttf");

    Shader vShader(vertShSrc, Shader::Type::Vertex);
    Shader fShader(fragShSrc, Shader::Type::Fragment);
    mProgram = std::make_unique<ShaderProgram>(vShader.Id(), fShader.Id());
}

void ConsoleRenderer::Shutdown()
{
    mTextRenderer = nullptr;
}

void ConsoleRenderer::Render(const Console& c, int width, int height)
{
    (void) width;
    // Store previous blending state
    GLboolean blend = glIsEnabled(GL_BLEND);

    // Store previous src alpha blend function
    GLint srcAlphaBlendFunc;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlphaBlendFunc);

    // Store previous depth test state
    GLint depthTest;
    glGetIntegerv(GL_DEPTH_TEST, &depthTest);
    glDisable(GL_DEPTH_TEST);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Actual rendering
    glUseProgram(mProgram->Id());
    {
        const std::string& cmdbuf = c.GetCommandBuffer();
        int xOffset = 4;
        int yOffset = height - 24;

        glEnable(GL_SCISSOR_TEST);
        glScissor(xOffset, yOffset, width - 8, 20);
        RenderQuad();
        glDisable(GL_SCISSOR_TEST);

        mTextRenderer->RenderText(
            cmdbuf,
            xOffset + 2, yOffset + 4, 12,
            glm::vec3(0.1f, 0.2f, 0.7f),
            "atari"
        );
    }
    glUseProgram(0);

    // Restore previous depth test state
    if(depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    // Restore blending options
    glBlendFunc(GL_SRC_ALPHA, srcAlphaBlendFunc);
    if(blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}
