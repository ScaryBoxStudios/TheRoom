#include "Game.hpp"
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
WARN_GUARD_ON
#include "../Graphics/Image/Jpeg/JpegLoader.hpp"
#include "../Graphics/Image/Png/Png.hpp"
#include "../Graphics/Model/ModelLoader.hpp"
#include <png++/png.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"
#include "Cube.hpp"

///==============================================================
///= GL Helpers
///==============================================================
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
            if(k == Key::R && ka == KeyAction::Release)
                mRenderData.rotating = !mRenderData.rotating;
        }
    );

    mRenderData.degreesInc = 0.05f;
    mRenderData.rotating = false;

    mCamera.SetPos(glm::vec3(0, 0, 8));

    // Load the Cube
    Mesh cubeData;
    cubeData.vertices.assign(std::begin(cubeVertexes), std::end(cubeVertexes));
    cubeData.indices.assign(std::begin(cubeIndices), std::end(cubeIndices));
    cubeData.colors.assign(std::begin(cubeColorData), std::end(cubeColorData));
    cubeData.texCoords.assign(std::begin(cubeTextureUVMappings), std::end(cubeTextureUVMappings));
    cubeData.normals.assign(std::begin(cubeNormals), std::end(cubeNormals));
    Model cube;
    cube.meshes.push_back(std::move(cubeData));
    mModelStore.Load("cube", cube);

    // Load the teapot
    ModelLoader modelLoader;
    // BufferType for the files loaded
    using BufferType = std::vector<std::uint8_t>;
    // Load shader files
    auto teapotFile = FileLoad<BufferType>("ext/teapot.obj");
    Model teapot = modelLoader.Load(*teapotFile);
    mModelStore.Load("teapot", std::move(teapot));

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
    for (std::size_t i = 0; i < cubePositions.size(); ++i)
    {
        const auto& pos = cubePositions[i];

        Transform trans;
        trans.Move(pos);
        trans.RotateX(20.0f * i);
        trans.RotateY(7.0f * i);
        trans.RotateZ(10.0f * i);
        mWorld.push_back({trans, "cube", "cube"});
    }

    // Add cube lights
    {
        Transform trans;
        trans.Move(glm::vec3(4.0f, 0.0f, 0.0f));
        trans.Scale(glm::vec3(0.3f));
        trans.RotateY(-10.0f);
        mWorld.push_back({trans, "cube", "light"});
        mLight = &mWorld.back();
    }

    // Add teapot
    {
        Transform trans;
        trans.Move(glm::vec3(0.0f, 4.0f, -5.0f));
        trans.Scale(glm::vec3(0.5f));
        mWorld.push_back({trans, "teapot", "light"});
    }

    GLInit();
    mGLData.drawMode = GL_FILL;
}

void Game::GLInit()
{
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
    auto lightVertFile = FileLoad<BufferType>("res/light_vert.glsl");
    auto lightFragFile = FileLoad<BufferType>("res/light_frag.glsl");

    // Convert them to std::string containers and get the raw pointer to their start
    std::string vShaderSrc((*vertFile).begin(), (*vertFile).end());
    std::string fShaderSrc((*fragFile).begin(), (*fragFile).end());
    std::string vLightShSrc((*lightVertFile).begin(), (*lightVertFile).end());
    std::string fLightShSrc((*lightFragFile).begin(), (*lightFragFile).end());

    // Compile shaders
    GLuint vShId = mShaderStore.LoadShader(vShaderSrc, ShaderStore::ShaderType::Vertex);
    GLuint fShId = mShaderStore.LoadShader(fShaderSrc, ShaderStore::ShaderType::Fragment);
    GLuint vLightShId = mShaderStore.LoadShader(vLightShSrc, ShaderStore::ShaderType::Vertex);
    GLuint fLightShId = mShaderStore.LoadShader(fLightShSrc, ShaderStore::ShaderType::Fragment);

    // Link them into a shader program
    mShaderStore.LinkProgram("cube", vShId, fShId);
    mShaderStore.LinkProgram("light", vLightShId, fLightShId);

    // Load the file contents into memory buffer
    std::unique_ptr<BufferType> imgData = FileLoad<BufferType>("ext/mahogany_wood.jpg");
    // Parse them using the JpegLoader
    JpegLoader jL;
    RawImage<> pb = jL.Load(*imgData);

    // Parse them using the PngLoader
    //png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>> img("ext/tree.png");
    //auto pb = img.get_pixbuf();

    // Store the texture in the store
    mTextureStore.Load("mahogany_wood", pb);
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

void Game::CalcLightPos()
{
    auto& trans = mLight->transform;
    float increase = 0.3f;
    if(mWindow.IsKeyPressed(Key::Kp8))
        trans.Move(glm::vec3(0.0f, -increase, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp4))
        trans.Move(glm::vec3(-increase, 0.0f, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp2))
        trans.Move(glm::vec3(0.0f, increase, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp6))
        trans.Move(glm::vec3(increase, 0.0f, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp5))
        trans.Move(glm::vec3(0.0f, 0.0f, -increase));
    if(mWindow.IsKeyPressed(Key::Kp0))
        trans.Move(glm::vec3(0.0f, 0.0f, increase));
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

    // Update interpolation variables
    for (auto& gObj : mWorld)
        gObj.transform.Update();

    // Update light position
    CalcLightPos();

    // Update cubes' rotations
    if (mRenderData.rotating)
    {
        for (auto& gObj : mWorld)
            if (gObj.type == "cube")
                gObj.transform.RotateY(mRenderData.degreesInc);
    }
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
        // Convinience reference
        const auto& gObj = mWorld[i];

        // Use the appropriate program
        GLuint progId = mShaderStore[gObj.type];
        glUseProgram(progId);

        // Calculate the model matrix
        glm::mat4 model = gObj.transform.GetInterpolated(interpolation);

        if (gObj.type == "cube")
        {
            // Bind the texture
            TextureDescription* tex = mTextureStore["mahogany_wood"];
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex->texId);
            GLuint samplerId = glGetUniformLocation(progId, "tex");
            glUniform1i(samplerId, 0);
        }

        // Upload the light position for the cubes' diffuse lighting
        if (gObj.type == "cube")
        {
            const glm::mat4& lTrans = mLight->transform.GetInterpolated(interpolation);
            const glm::vec3 lightPos = glm::vec3(lTrans[3].x, lTrans[3].y, lTrans[3].z);
            GLint lightPosId = glGetUniformLocation(progId, "lightPos");
            glUniform3f(lightPosId, lightPos.x, lightPos.y, lightPos.z);

            const auto& viewPos = cameraPos;
            GLint viewPosId = glGetUniformLocation(progId, "viewPos");
            glUniform3f(viewPosId, viewPos.x, viewPos.y, viewPos.z);
        }

        // Upload projection, view and model matrices as uniforms
        auto projectionId = glGetUniformLocation(progId, "projection");
        glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(projection));
        auto viewId = glGetUniformLocation(progId, "view");
        glUniformMatrix4fv(viewId, 1, GL_FALSE, glm::value_ptr(view));
        auto modelId = glGetUniformLocation(progId, "model");
        glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(model));

        // Get the mesh
        ModelDescription* mdl = mModelStore[gObj.model];

        // Draw the object
        for (const auto& mesh : mdl->meshes)
        {
            glBindVertexArray(mesh.vaoId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eboId);
            glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        }
    }

    // Check for errors
    CheckGLError();

    // Show it
    mWindow.SwapBuffers();
}

void Game::Shutdown()
{
    // Explicitly deallocate GPU data
    mTextureStore.Clear();
    mModelStore.Clear();
    mShaderStore.Clear();

    // Window
    mWindow.Destroy();
}

void Game::SetExitHandler(std::function<void()> f)
{
    mExitHandler = f;
}

