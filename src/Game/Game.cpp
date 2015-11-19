#include "Game.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../Graphics/Image/PixelTraits.hpp"
#include "../Graphics/Image/PixelBufferTraits.hpp"
WARN_GUARD_ON
#include "../Graphics/Image/Png/Png.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <png++/png.hpp>
WARN_GUARD_OFF

///==============================================================
///= Const Data
///==============================================================
const GLfloat vertexBufferData[] = {
     // Front
    -1.0, -1.0,  1.0, 1,
     1.0, -1.0,  1.0, 1,
     1.0,  1.0,  1.0, 1,
    -1.0,  1.0,  1.0, 1,
     // Top
    -1.0,  1.0,  1.0, 1,
     1.0,  1.0,  1.0, 1,
     1.0,  1.0, -1.0, 1,
    -1.0,  1.0, -1.0, 1,
     // Back
     1.0, -1.0, -1.0, 1,
    -1.0, -1.0, -1.0, 1,
    -1.0,  1.0, -1.0, 1,
     1.0,  1.0, -1.0, 1,
     // Bottom
    -1.0, -1.0, -1.0, 1,
     1.0, -1.0, -1.0, 1,
     1.0, -1.0,  1.0, 1,
    -1.0, -1.0,  1.0, 1,
     // Left
    -1.0, -1.0, -1.0, 1,
    -1.0, -1.0,  1.0, 1,
    -1.0,  1.0,  1.0, 1,
    -1.0,  1.0, -1.0, 1,
     // Right
     1.0, -1.0,  1.0, 1,
     1.0, -1.0, -1.0, 1,
     1.0,  1.0, -1.0, 1,
     1.0,  1.0,  1.0, 1
};

const GLfloat colorBufferData[] = {
    0, 0, 1,
    0, 1, 0,
    0, 1, 1,
    1, 0, 0,
    1, 0, 1,
    1, 1, 0,
    0, 0, 1,
    0, 1, 0,
    0, 1, 1,
    1, 0, 0,
    1, 0, 1,
    1, 1, 0,
    0, 0, 1,
    0, 1, 0,
    0, 1, 1,
    1, 0, 0,
    1, 0, 1,
    1, 1, 0,
    0, 0, 1,
    0, 1, 0,
    0, 1, 1,
    1, 0, 0,
    1, 0, 1,
    1, 1, 0
};

const GLubyte indices[] = {
    // Front
    0, 1, 2,
    2, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Back
    8, 9, 10,
    10, 11, 8,
    // Bottom
    12, 13, 14,
    14, 15, 12,
    // Left
    16, 17, 18,
    18, 19, 16,
    // Right
    20, 21, 22,
    22, 23, 20
};

const GLfloat textureCoords[] = {
    // Front
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    // Top
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    // Back
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    // Bottom
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    // Left
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    // Right
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};

const GLchar* vertexShader = R"rsd(

#version 330

in vec2 in_texCoords;
layout(location=0) in vec4 in_Pos;
layout(location=1) in vec3 in_Col;
out vec3 vertexColor;
out vec2 out_texCoords;

uniform mat4 MVP;

void main(void)
{
    out_texCoords = in_texCoords;
    gl_Position = MVP * in_Pos;
    vertexColor = in_Col;
}

)rsd";

const GLchar* fragmentShader = R"rsd(

#version 330

in vec3 vertexColor;
in vec2 out_texCoords;

out vec4 color;

uniform sampler2D tex;

void main(void)
{
    color = texture(tex, out_texCoords) * vec4(vertexColor, 1);
}

)rsd";

///==============================================================
///= GL Helpers
///==============================================================
static void CompileShader(GLuint shaderId)
{
    glCompileShader(shaderId);

    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            std::vector<GLchar> buf(logLength, 0);
            glGetShaderInfoLog(shaderId, logLength, 0, buf.data());
            //Error("Shader Compilation Error!", buf.data());
        }
    }
}

static void LinkProgram(GLuint programId)
{
    glLinkProgram(programId);

    GLint linkStatus;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            std::vector<GLchar> buf(logLength, 0);
            glGetShaderInfoLog(programId, logLength, 0, buf.data());
            //Error("GL Program Link Error!", buf.data());
        }
    }
}

template <typename PixelBuffer>
void SetTextureData(PixelBuffer pb)
{
    GLint format = PixelTraits<typename PixelBufferTraits<PixelBuffer>::Pixel>::GetChannels() == 4 ? GL_RGBA : GL_RGB;
    size_t width = PixelBufferTraits<PixelBuffer>::Width(pb);
    size_t height = PixelBufferTraits<PixelBuffer>::Height(pb);
    const GLvoid* data = PixelBufferTraits<PixelBuffer>::GetData(pb);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

static void CheckGLError()
{
    GLenum errVal = glGetError();
    if (errVal != GL_NO_ERROR)
    {
        std::stringstream ss;
        ss << "OpenGL Error! Code: " << errVal;
        const char* desc = reinterpret_cast<const char*>(gluErrorString(errVal));
        (void) desc;
        //Error(ss.str().c_str(), desc);
    }
}

///==============================================================
///= Game
///==============================================================
Game::Game()
{
}

void Game::Init()
{
    mWindow.Create(800, 600, "TheRoom", Window::Mode::Windowed);
    mWindow.SetShowFPS(true);
    mWindow.SetCloseHandler(mExitHandler);
    mWindow.SetMouseButtonPressHandler(
        [this](MouseButton mb, ButtonAction ba)
        {
            if (mb == MouseButton::Left && ba == ButtonAction::Press)
                mWindow.SetMouseGrabEnabled(true);
        }
    );
    mWindow.SetKeyPressedHandler(
        [this](Key k, KeyAction ka)
        {
            // Exit
            if(k == Key::Escape && ka == KeyAction::Release)
                mExitHandler();
            // Ungrab mouse
            if(k == Key::RightControl && ka == KeyAction::Release)
                mWindow.SetMouseGrabEnabled(false);
        }
    );
    mWindow.SetCursorPositionChangedHandler(
        [this](double x, double y)
        {
            if (mWindow.MouseGrabEnabled())
            {
                mCamera.xOffset = x - mCamera.lastX;
                mCamera.yOffset = mCamera.lastY - y;
                mCamera.lastX = x;
                mCamera.lastY = y;
            }
        }
    );

    mCamera.up = glm::vec3(0, 1, 0);
    mCamera.front = glm::vec3(0, 0, -1);
    mCamera.pos = glm::vec3(0, 3, 8);
    mCamera.speed = 0.3f;
    mCamera.sensitivity = 0.05;
    mCamera.yaw = -90.0f;
    mCamera.pitch = -20.0f;
    mCamera.xOffset = 0.0f;
    mCamera.yOffset = 0.0f;

    mRenderData.degrees = 0.1f;
    mRenderData.degreesInc = 0.05f;

    GLInit();
}

void Game::GLInit()
{
    // Generate the various resources
    glGenBuffers(1, &mGLData.vBuf);
    glGenBuffers(1, &mGLData.colBuf);
    glGenBuffers(1, &mGLData.idxBuf);
    glGenBuffers(1, &mGLData.texCoordBuf);
    glGenTextures(1, &mGLData.tex);
    mGLData.programId = glCreateProgram();

    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable the depth buffer
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LESS);
    
    // Create VAO
    glGenVertexArrays(1, &mGLData.vaoId);
    glBindVertexArray(mGLData.vaoId);

    // Create VBOs
    glBindBuffer(GL_ARRAY_BUFFER, mGLData.vBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Color buffer
    glBindBuffer(GL_ARRAY_BUFFER, mGLData.colBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGLData.idxBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Compile and Link shaders
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexShader, 0);
    CompileShader(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentShader, 0);
    CompileShader(fShader);

    glAttachShader(mGLData.programId, vShader);
    glAttachShader(mGLData.programId, fShader);
    LinkProgram(mGLData.programId);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    
    glUseProgram(mGLData.programId);

    // Create texture coordinate buffer
    glBindBuffer(GL_ARRAY_BUFFER, mGLData.texCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
    GLint texAttrib = glGetAttribLocation(mGLData.programId, "in_texCoords");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Load the sample texture
    glActiveTexture(GL_TEXTURE0);
    png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>> img("ext/tree.png");
    auto pb = img.get_pixbuf();
    glBindTexture(GL_TEXTURE_2D, mGLData.tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SetTextureData(pb);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Pass it to OpenGL
    GLuint samplerId = glGetUniformLocation(mGLData.programId, "tex");
    glUniform1i(samplerId, 0);

    mGLData.matrixId = glGetUniformLocation(mGLData.programId, "MVP");
    glBindVertexArray(0);
    CheckGLError();
}

glm::vec3 calcCamFront(float yaw, float pitch)
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(front);
}

void Game::Update(float dt)
{
    (void) dt;

    // Poll window events
    mWindow.PollEvents();

    // Update camera euler angles
    auto& xOffset = mCamera.xOffset;
    auto& yOffset = mCamera.yOffset;
    auto& yaw = mCamera.yaw;
    auto& pitch = mCamera.pitch;

    xOffset *= mCamera.sensitivity;
    yOffset *= mCamera.sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    GLfloat pitchLim = 30.0f; // Normal: 89.0f
    if(pitch > pitchLim)
        pitch = pitchLim;
    if(pitch < -pitchLim)
        pitch = -pitchLim;

    mCamera.front = calcCamFront(yaw, pitch);

    // Update camera position
    if(mWindow.IsKeyPressed(Key::W))
        mCamera.pos += mCamera.speed * mCamera.front;
    if(mWindow.IsKeyPressed(Key::A))
        mCamera.pos -= glm::normalize(glm::cross(mCamera.front, mCamera.up)) * mCamera.speed;
    if(mWindow.IsKeyPressed(Key::S))
        mCamera.pos -= mCamera.speed * mCamera.front;
    if(mWindow.IsKeyPressed(Key::D))
        mCamera.pos += glm::normalize(glm::cross(mCamera.front, mCamera.up)) * mCamera.speed;

    // Update state
    mRenderData.degrees += mRenderData.degreesInc;
}

void Game::Render(float interpolation)
{
    // Render stuff
    glBindVertexArray(mGLData.vaoId);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    // View calculation with camera
    glm::vec3 cameraPos = mCamera.pos;
    glm::vec3 cameraFront = mCamera.front;
        // Interpolate camera positioning
        if(mWindow.IsKeyPressed(Key::W))
            cameraPos += mCamera.speed * mCamera.front * interpolation;
        if(mWindow.IsKeyPressed(Key::A))
            cameraPos -= glm::normalize(glm::cross(mCamera.front, mCamera.up)) * mCamera.speed * interpolation;
        if(mWindow.IsKeyPressed(Key::S))
            cameraPos -= mCamera.speed * mCamera.front * interpolation;
        if(mWindow.IsKeyPressed(Key::D))
            cameraPos += glm::normalize(glm::cross(mCamera.front, mCamera.up)) * mCamera.speed * interpolation;
        // TODO: Interpolate look around

    glm::vec3 cameraUp = mCamera.up;
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, mRenderData.degrees + mRenderData.degreesInc * interpolation, glm::vec3(0, 1, 0));

    glm::mat4 MVP = projection * view * model;

    glUniformMatrix4fv(mGLData.matrixId, 1, GL_FALSE, glm::value_ptr(MVP));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
    glBindVertexArray(0);
    CheckGLError();
   
    // Show it
    mWindow.SwapBuffers();
}

void Game::Shutdown()
{
    // OpenGL
    glUseProgram(0);

    // Disable attribute arrays
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDeleteBuffers(1, &mGLData.vBuf);
    glDeleteBuffers(1, &mGLData.colBuf);
    glDeleteBuffers(1, &mGLData.idxBuf);
    glDeleteBuffers(1, &mGLData.texCoordBuf);
    glDeleteTextures(1, &mGLData.tex);
    glDeleteProgram(mGLData.programId);

    // Delete VAO
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &mGLData.vaoId);

    // Window
    mWindow.Destroy();
}

void Game::SetExitHandler(std::function<void()> f)
{
    mExitHandler = f;
}

