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

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

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
    // Setup window
    mWindow.Create(800, 600, "TheRoom", Window::Mode::Windowed);
    mWindow.SetShowFPS(true);
    mWindow.SetCloseHandler(mExitHandler);

    // Setup input event handlers
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
                if (mDrawMode == GL_FILL)
                    mDrawMode = GL_POINT;
                else if (mDrawMode == GL_POINT)
                    mDrawMode = GL_LINE;
                else if (mDrawMode == GL_LINE)
                    mDrawMode = GL_FILL;
            }
            if(k == Key::R && ka == KeyAction::Release)
                mRotationData.rotating = !mRotationData.rotating;
        }
    );

    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable the depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    mDrawMode = GL_FILL;

    // Cube rotation state
    mRotationData.degreesInc = 0.05f;
    mRotationData.rotating = false;

    // Camera initial position
    mCamera.SetPos(glm::vec3(0, 0, 8));

    //
    // Textures
    //
    // The JpegLoader object
    JpegLoader jL;

    // The texture map
    std::unordered_map<std::string, std::string> textures =
    {
        {"ext/mahogany_wood.jpg", "mahogany_wood"},
        {"ext/mahogany_wood_spec.jpg", "mahogany_wood_spec"},
        {"ext/WoodenCabin/WoodCabinDif.jpg", "house_diff"},
        {"ext/WoodenCabin/WoodCabinSM.jpg", "house_spec"},
    };

    // Load the textures
    for (const auto& p : textures)
    {
        auto textureFile = FileLoad<BufferType>(p.first);
        RawImage<> pb = jL.Load(*textureFile);
        mTextureStore.Load(p.second, pb);
    }

    //
    // Models
    //
    // Model loader instance
    ModelLoader modelLoader;
    // Load the cube
    auto cubeFile = FileLoad<BufferType>("ext/Cube/cube.obj");
    Model cube = modelLoader.Load(*cubeFile, "obj");
    mModelStore.Load("cube", std::move(cube));
    mModelStore["cube"]->diffTexId = mTextureStore["mahogany_wood"]->texId;
    mModelStore["cube"]->specTexId = mTextureStore["mahogany_wood_spec"]->texId;

    // Load teapot
    auto teapotFile = FileLoad<BufferType>("ext/teapot.obj");
    Model teapot = modelLoader.Load(*teapotFile, "obj");
    mModelStore.Load("teapot", std::move(teapot));

    // Load house
    auto houseFile = FileLoad<BufferType>("ext/WoodenCabin/WoodenCabin.dae");
    Model house = modelLoader.Load(*houseFile, "dae");
    mModelStore.Load("house", std::move(house));
    mModelStore["house"]->diffTexId = mTextureStore["house_diff"]->texId;
    mModelStore["house"]->specTexId = mTextureStore["house_spec"]->texId;

    //
    // Normal objects
    //
    std::vector<GameObject> normalObjects;
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
        trans.Scale(glm::vec3(2.0f));
        trans.RotateX(20.0f * i);
        trans.RotateY(7.0f * i);
        trans.RotateZ(10.0f * i);
        normalObjects.push_back({trans, "cube"});
    }

    // Add house
    {
        Transform trans;
        trans.Move(glm::vec3(0.0f, -10.0f, -40.0f));
        trans.Scale(glm::vec3(0.3f));
        normalObjects.push_back({trans, "house"});
    }
    mWorld.insert({"normal", normalObjects});

    //
    // Light objects
    //
    std::vector<GameObject> lightObjects;
    // Add teapot
    {
        Transform trans;
        trans.Move(glm::vec3(0.0f, 4.0f, -5.0f));
        trans.Scale(glm::vec3(0.5f));
        lightObjects.push_back({trans, "teapot"});
    }

    // Add cube lights
    {
        Transform trans;
        trans.Move(glm::vec3(4.0f, 0.0f, 0.0f));
        trans.RotateY(-10.0f);
        lightObjects.push_back({trans, "cube"});
    }
    mWorld.insert({"light", lightObjects});
    mLight = &mWorld["light"].back();

    // Load shader files
    auto vertFile = FileLoad<BufferType>("res/normal_vert.glsl");
    auto fragFile = FileLoad<BufferType>("res/normal_frag.glsl");
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
    mShaderStore.LinkProgram("normal", vShId, fShId);
    mShaderStore.LinkProgram("light", vLightShId, fLightShId);

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
        trans.Move(glm::vec3(0.0f, increase, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp4))
        trans.Move(glm::vec3(increase, 0.0f, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp2))
        trans.Move(glm::vec3(0.0f, -increase, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp6))
        trans.Move(glm::vec3(-increase, 0.0f, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp5))
        trans.Move(glm::vec3(0.0f, 0.0f, increase));
    if(mWindow.IsKeyPressed(Key::Kp0))
        trans.Move(glm::vec3(0.0f, 0.0f, -increase));
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
    for (auto& p : mWorld)
        for (auto& gObj : p.second)
            gObj.transform.Update();

    // Update light position
    CalcLightPos();

    // Update cubes' rotations
    if (mRotationData.rotating)
    {
        for (auto& p : mWorld)
            if (p.first == "normal")
                for (auto& gObj : p.second)
                    gObj.transform.RotateY(mRotationData.degreesInc);
    }
}

void Game::Render(float interpolation)
{
    // Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, mDrawMode);

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

    for (const auto& objCategory : mWorld)
    {
        // The object material category iterating through
        const auto& type = objCategory.first;

        // Use the appropriate program
        GLuint progId = mShaderStore[type];
        glUseProgram(progId);

        if (type == "normal")
        {
            // Setup lighting position parameters
            const glm::mat4& lTrans = mLight->transform.GetInterpolated(interpolation);
            const glm::vec3 lightPos = glm::vec3(lTrans[3].x, lTrans[3].y, lTrans[3].z);
            GLint lightPosId = glGetUniformLocation(progId, "light.position");
            glUniform3f(lightPosId, lightPos.x, lightPos.y, lightPos.z);

            const auto& viewPos = cameraPos;
            GLint viewPosId = glGetUniformLocation(progId, "viewPos");
            glUniform3f(viewPosId, viewPos.x, viewPos.y, viewPos.z);

            // Set lights properties
            glUniform3f(glGetUniformLocation(progId, "light.ambient"),  0.2f, 0.2f, 0.2f);
            glUniform3f(glGetUniformLocation(progId, "light.diffuse"),  0.5f, 0.5f, 0.5f);
            glUniform3f(glGetUniformLocation(progId, "light.specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(progId, "light.constant"), 1.0f);
            glUniform1f(glGetUniformLocation(progId, "light.linear"), 0.09f);
            glUniform1f(glGetUniformLocation(progId, "light.quadratic"), 0.032f);

            // Set material properties
            glUniform1f(glGetUniformLocation(progId, "material.shininess"), 32.0f);
        }

        // Upload projection and view matrices
        auto projectionId = glGetUniformLocation(progId, "projection");
        glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(projection));
        auto viewId = glGetUniformLocation(progId, "view");
        glUniformMatrix4fv(viewId, 1, GL_FALSE, glm::value_ptr(view));

        // The actual object list
        const auto& list = objCategory.second;
        for (const auto& gObj : list)
        {
            // Calculate the model matrix
            glm::mat4 model = gObj.transform.GetInterpolated(interpolation);

            // Upload it
            auto modelId = glGetUniformLocation(progId, "model");
            glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(model));

            // Get the model
            ModelDescription* mdl = mModelStore[gObj.model];

            if (type == "normal")
            {
                // Bind the needed textures
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mdl->diffTexId);
                GLuint diffuseId = glGetUniformLocation(progId, "material.diffuse");
                glUniform1i(diffuseId, 0);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mdl->specTexId);
                GLuint specId = glGetUniformLocation(progId, "material.specular");
                glUniform1i(specId, 1);
            }

            // Draw all its meshes
            for (const auto& mesh : mdl->meshes)
            {
                glBindVertexArray(mesh.vaoId);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eboId);
                glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }

        glUseProgram(0);
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

