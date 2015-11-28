#include "Game.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../Graphics/Image/PixelTraits.hpp"
#include "../Graphics/Image/PixelBufferTraits.hpp"
#include "../Graphics/Image/RawImageTraits.hpp"
WARN_GUARD_ON
#include "../Graphics/Image/Jpeg/JpegLoader.hpp"
#include "../Graphics/Image/Png/Png.hpp"
#include <png++/png.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"
#include "Cube.hpp"

///==============================================================
///= GL Helpers
///==============================================================
template <typename PixelBuffer>
void SetTextureData(PixelBuffer pb)
{
    GLint format = PixelTraits<typename PixelBufferTraits<PixelBuffer>::Pixel>::GetChannels() == 4 ? GL_RGBA : GL_RGB;
    uint32_t width = PixelBufferTraits<PixelBuffer>::Width(pb);
    uint32_t height = PixelBufferTraits<PixelBuffer>::Height(pb);
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
            // Toggle polygon mode
            if(k == Key::P && ka == KeyAction::Release)
            {
                if (mGLData.drawMode == GL_FILL)
                    mGLData.drawMode = GL_POINT;
                else if (mGLData.drawMode == GL_POINT)
                    mGLData.drawMode = GL_LINE;
                else if (mGLData.drawMode == GL_LINE)
                    mGLData.drawMode = GL_FILL;
            }
        }
    );

    mRenderData.degrees = 0.1f;
    mRenderData.degreesInc = 0.05f;

    mCamera.SetPos(glm::vec3(0, 0, 8));

    // Load the Cube
    ModelData cubeData;
    cubeData.vertices.assign(std::begin(cubeVertexes), std::end(cubeVertexes));
    cubeData.indices.assign(std::begin(cubeIndices), std::end(cubeIndices));
    cubeData.colors.assign(std::begin(cubeColorData), std::end(cubeColorData));
    cubeData.texCoords.assign(std::begin(cubeTextureUVMappings), std::end(cubeTextureUVMappings));

    std::unique_ptr<Model> cube = std::make_unique<Model>();
    Model* cubePtr = cube.get();
    cube->Load(cubeData);
    mModelStore["cube"] = std::move(cube);

    // Create various Cube instances in the world
    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(4.0f, 10.0f, -20.0f),
        glm::vec3(-3.0f, -4.4f, -5.0f),
        glm::vec3(-7.6f, -4.0f, -14.0f),
        glm::vec3(4.4f, -3.5f, -4.0f),
        glm::vec3(-3.4f, 6.0f, -15.0f),
        glm::vec3(2.6f, -4.0f, -17.0f),
        glm::vec3(4.0f, 3.0f, -5.0f),
        glm::vec3(3.0f, 0.4f, -12.0f),
        glm::vec3(-3.5f, 2.0f, -3.0f)
    };
    for (const auto& pos : cubePositions)
        mWorld.push_back({pos, cubePtr});

    CheckGLError();
    GLInit();
    mGLData.drawMode = GL_FILL;
}

void Game::GLInit()
{
    // Generate the various resources
    glGenTextures(1, &mGLData.tex);

    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable the depth buffer
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LESS);

    // BufferType for the files loaded
    using BufferType = std::vector<std::uint8_t>;

    // Load shader files
    auto vertFile = FileLoad<BufferType>("res/cube_vert.glsl");
    auto fragFile = FileLoad<BufferType>("res/cube_frag.glsl");

    // Convert them to std::string containers and get the raw pointer to their start
    std::string vShaderSrc((*vertFile).begin(), (*vertFile).end());
    std::string fShaderSrc((*fragFile).begin(), (*fragFile).end());

    // Compile shaders
    Shader vert(Shader::Type::Vertex);
    vert.Source(vShaderSrc);
    Shader frag(Shader::Type::Fragment);
    frag.Source(fShaderSrc);

    // Link them into a shader program
    std::unique_ptr<ShaderProgram> prog = std::make_unique<ShaderProgram>();
    prog->Link(vert.Id(), frag.Id());
    mShaderProgramStore["cube"] = std::move(prog);

    // Load the file contents into memory buffer
    std::unique_ptr<BufferType> imgData = FileLoad<BufferType>("ext/mahogany_wood.jpg");
    // Parse them using the JpegLoader
    JpegLoader jL;
    RawImage<> pb = jL.Load(*imgData);

    // Parse them using the PngLoader
    //png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>> img("ext/tree.png");
    //auto pb = img.get_pixbuf();

    // Upload
    glBindTexture(GL_TEXTURE_2D, mGLData.tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SetTextureData(pb);
    glGenerateMipmap(GL_TEXTURE_2D);

    CheckGLError();
}

std::vector<Camera::MoveDirection> Game::CameraMoveDirections()
{
    std::vector<Camera::MoveDirection> mds;
    if(mWindow.IsKeyPressed(Key::W))
        mds.push_back(Camera::MoveDirection::Forward);
    if(mWindow.IsKeyPressed(Key::A))
        mds.push_back(Camera::MoveDirection::Left);
    if(mWindow.IsKeyPressed(Key::S))
        mds.push_back(Camera::MoveDirection::BackWard);
    if(mWindow.IsKeyPressed(Key::D))
        mds.push_back(Camera::MoveDirection::Right);
    return mds;
}

std::tuple<float, float> Game::CameraLookOffset()
{
    std::tuple<double, double> curDiff = mWindow.GetCursorDiff();
    return std::make_tuple(
        static_cast<float>(std::get<0>(curDiff)),
        static_cast<float>(std::get<1>(curDiff))
    );
}

void Game::Update(float dt)
{
    (void) dt;

    // Poll window events
    mWindow.Update();

    // Update camera euler angles
    if (mWindow.MouseGrabEnabled())
        mCamera.Look(CameraLookOffset());

    mCamera.Move(CameraMoveDirections());

    // Update state
    mRenderData.degrees += mRenderData.degreesInc;
}

void Game::Render(float interpolation)
{
    // Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, mGLData.drawMode);

    // Create the projection matrix
    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    // View calculation with camera
    auto lookOffset = mWindow.MouseGrabEnabled() ? CameraLookOffset() : std::make_tuple(0.0f, 0.0f);
    auto iCamState = mCamera.Interpolate(CameraMoveDirections(), lookOffset, interpolation);
    glm::vec3& cameraPos = iCamState.position;
    glm::vec3& cameraFront = iCamState.front;
    glm::vec3& cameraUp = iCamState.up;

    // Create the view matrix
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    for (std::size_t i = 0; i < mWorld.size(); ++i)
    {
        const auto& gObj = mWorld[i];

        // Calculate the model matrix
        glm::mat4 model;
        model = glm::translate(model, gObj.position);
        model = glm::rotate(model, mRenderData.degrees + mRenderData.degreesInc * interpolation, glm::vec3(0, 1, 0));
        model = glm::rotate(model, 20.0f * i, glm::vec3(1.0f, 0.3f, 0.5f));

        // Use the appropriate program
        auto& p = mShaderProgramStore["cube"];
        glUseProgram(p->Id());

        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mGLData.tex);
        GLuint samplerId = glGetUniformLocation(p->Id(), "tex");
        glUniform1i(samplerId, 0);

        // Combine the projection, view and model matrices
        glm::mat4 MVP = projection * view * model;
        // Upload the combined matrix as a uniform
        auto matrixId = glGetUniformLocation(p->Id(), "MVP");
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, glm::value_ptr(MVP));

        // Draw the object
        glBindVertexArray(gObj.model->VaoId());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gObj.model->EboId());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    // Check for errors
    CheckGLError();

    // Show it
    mWindow.SwapBuffers();
}

void Game::Shutdown()
{
    glDeleteTextures(1, &mGLData.tex);

    // Window
    mWindow.Destroy();
}

void Game::SetExitHandler(std::function<void()> f)
{
    mExitHandler = f;
}

