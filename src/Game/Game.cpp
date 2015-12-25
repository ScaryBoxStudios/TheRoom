#include "Game.hpp"
#include "../Window/GlfwError.hpp"
#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Graphics/Image/ImageLoader.hpp"
#include "../Graphics/Model/ModelLoader.hpp"
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"

///==============================================================
///= Game
///==============================================================
// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

Game::Game()
{
}

void Game::Init()
{
    // Setup window and input
    SetupWindow();

    // Initialize the renderer
    renderer.Init(mWindow.GetWidth(), mWindow.GetHeight());

    // Cube rotation state
    mRotationData.degreesInc = 0.05f;
    mRotationData.rotating = false;

    // Camera initial position
    mCamera.SetPos(glm::vec3(0, 0, 8));

    // Load the textures
    LoadTextures();

    // Load the models
    LoadModels();

    // Load the shaders
    LoadShaders();

    // Create world objects
    SetupWorld();
}

void Game::SetupWorld()
{
    // Retrieve the world from the renderer
    auto& world = renderer.GetWorld();

    //
    // Normal objects
    //
    std::vector<Renderer::WorldObject> normalObjects;
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
    world.insert({"normal", normalObjects});

    //
    // Light objects
    //
    std::vector<Renderer::WorldObject> lightObjects;

    // Add cube lights
    {
        Transform trans;
        trans.Move(glm::vec3(4.0f, 0.0f, 0.0f));
        trans.Scale(glm::vec3(0.3f));
        lightObjects.push_back({trans, "teapot"});
    }
    world.insert({"light", lightObjects});
    renderer.mLight = &world["light"].back();
}

void Game::SetupWindow()
{
    // Setup window
    bool success = mWindow.Create(800, 600, "TheRoom", Window::Mode::Windowed);
    if (!success)
        throw std::runtime_error(GetLastGlfwError().GetDescription());

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
            if(k == Key::R && ka == KeyAction::Release)
                mRotationData.rotating = !mRotationData.rotating;
        }
    );
}

void Game::LoadShaders()
{
    // Retrieve the shaderStore from the renderer
    auto& shaderStore = renderer.GetShaderStore();

    // The shader map
    std::unordered_map<std::string, std::vector<std::pair<ShaderStore::ShaderType,std::string>>> shaders =
    {
        {
            "geometry_pass",
            {
                { ShaderStore::ShaderType::Vertex,   "res/geometry_pass_vert.glsl" },
                { ShaderStore::ShaderType::Fragment, "res/geometry_pass_frag.glsl" },
            }
        },
        {
            "light_pass",
            {
                { ShaderStore::ShaderType::Vertex,   "res/light_pass_vert.glsl" },
                { ShaderStore::ShaderType::Fragment, "res/light_pass_frag.glsl" },
            }
        }
    };

    // Load shaders
    for (const auto& p : shaders)
    {
        // Will temporarily store the id's of the compiled shaders
        std::unordered_map<ShaderStore::ShaderType, GLuint> shaderIds;
        for (const auto& f : p.second)
        {
            // Load file
            auto shaderFile = FileLoad<BufferType>(f.second);
            if (!shaderFile)
                throw std::runtime_error("Could not find shader file: \n" + f.second);
            // Convert it to std::string containter
            std::string shaderSrc((*shaderFile).begin(), (*shaderFile).end());
            GLuint sId = shaderStore.LoadShader(shaderSrc, f.first);
            if (sId == 0)
                throw std::runtime_error("Shader compilation error: \n" + shaderStore.GetLastCompileError());
            // Insert loaded shader id to temp map
            shaderIds.insert({f.first, sId});
        }
        // Fetch vert and frag shader id's from temp map and link
        bool s = shaderStore.LinkProgram(p.first,
            shaderIds[ShaderStore::ShaderType::Vertex],
            shaderIds[ShaderStore::ShaderType::Fragment]);
        if (!s)
            throw std::runtime_error("OpenGL program link error: \n" + shaderStore.GetLastLinkError());
    }
}

void Game::LoadTextures()
{
    // Retrieve the textureStore from the renderer
    auto& textureStore = renderer.GetTextureStore();

    // The ImageLoader object
    ImageLoader imageLoader;

    // The texture map
    std::unordered_map<std::string, std::string> textures =
    {
        {"ext/mahogany_wood.jpg",            "mahogany_wood"},
        {"ext/mahogany_wood_spec.jpg",       "mahogany_wood_spec"},
        {"ext/WoodenCabin/WoodCabinDif.jpg", "house_diff"},
        {"ext/WoodenCabin/WoodCabinSM.jpg",  "house_spec"},
    };

    // Load the textures
    for (const auto& p : textures)
    {
        RawImage<> pb = imageLoader.LoadFile(p.first);
        textureStore.Load(p.second, pb);
    }

    // Add calculated textures
    RawImage<> pb({0xFF, 0xFF, 0xFF}, {1, 1, 3});
    textureStore.Load("white", pb);
    RawImage<> pb2({0x00, 0x00, 0x00}, {1, 1, 3});
    textureStore.Load("white_spec", pb2);
}

void Game::LoadModels()
{
    // Retrieve the model and texture stores from the renderer
    auto& modelStore = renderer.GetModelStore();
    auto& textureStore = renderer.GetTextureStore();

    // Model loader instance
    ModelLoader modelLoader;

    // Load the cube
    auto cubeFile = FileLoad<BufferType>("ext/Cube/cube.obj");
    if(!cubeFile)
        throw std::runtime_error("Couldn't load file (ext/Cube/cube.obj)");

    Model cube = modelLoader.Load(*cubeFile, "obj");
    if(cube.meshes.size() == 0)
        throw std::runtime_error("Couldn't load model (ext/Cube/cube.obj)");

    modelStore.Load("cube", std::move(cube));
    modelStore["cube"]->diffTexId = textureStore["mahogany_wood"]->texId;
    modelStore["cube"]->specTexId = textureStore["mahogany_wood_spec"]->texId;

    // Load teapot
    auto teapotFile = FileLoad<BufferType>("ext/teapot.obj");
    if(!teapotFile)
        throw std::runtime_error("Couldn't load file (ext/teapot.obj)");

    Model teapot = modelLoader.Load(*teapotFile, "obj");
    if(teapot.meshes.size() == 0)
        throw std::runtime_error("Couldn't load model (ext/teapot.obj)");

    modelStore.Load("teapot", std::move(teapot));
    modelStore["teapot"]->diffTexId = textureStore["white"]->texId;
    modelStore["teapot"]->specTexId = textureStore["white_spec"]->texId;

    // Load house
    auto houseFile = FileLoad<BufferType>("ext/WoodenCabin/WoodenCabin.dae");
    if(!houseFile)
        throw std::runtime_error("Couldn't load file (ext/WoodenCabin/WoodenCabin.dae)");

    Model house = modelLoader.Load(*houseFile, "dae");
    if(house.meshes.size() == 0)
        throw std::runtime_error("Couldn't load model (ext/WoodenCabin/WoodenCabin.dae)");

    modelStore.Load("house", std::move(house));
    modelStore["house"]->diffTexId = textureStore["house_diff"]->texId;
    modelStore["house"]->specTexId = textureStore["house_spec"]->texId;
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

    // Update the interpolation state of the world
    renderer.Update(dt);

    // Update camera euler angles
    if (mWindow.MouseGrabEnabled())
        mCamera.Look(CameraLookOffset());

    // Update camera position
    mCamera.Move(CameraMoveDirections());

    // Update light position
    auto& trans = renderer.mLight->transform;
    float increase = 0.7f;
    if(mWindow.IsKeyPressed(Key::Kp8))
        trans.Move(glm::vec3(0.0f, increase, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp4))
        trans.Move(glm::vec3(-increase, 0.0f, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp2))
        trans.Move(glm::vec3(0.0f, -increase, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp6))
        trans.Move(glm::vec3(increase, 0.0f, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp5))
        trans.Move(glm::vec3(0.0f, 0.0f, -increase));
    if(mWindow.IsKeyPressed(Key::Kp0))
        trans.Move(glm::vec3(0.0f, 0.0f, increase));

    // Update cubes' rotations
    if (mRotationData.rotating)
    {
        for (auto& p : renderer.GetWorld())
            if (p.first == "normal")
                for (auto& gObj : p.second)
                    gObj.transform.RotateY(mRotationData.degreesInc);
    }
}

void Game::Render(float interpolation)
{
    // View calculation with camera
    auto lookOffset = mWindow.MouseGrabEnabled() ? CameraLookOffset() : std::make_tuple(0.0f, 0.0f);
    auto iCamState = mCamera.Interpolate(CameraMoveDirections(), lookOffset, interpolation);

    // Create the view matrix and pass it to the renderer
    glm::mat4 view = glm::lookAt(iCamState.position, iCamState.position + iCamState.front, iCamState.up);
    renderer.SetView(view);

    // Render
    renderer.Render(interpolation);

    // Show it
    mWindow.SwapBuffers();
}

void Game::Shutdown()
{
    // Renderer
    renderer.Shutdown();

    // Window
    mWindow.Destroy();
}

void Game::SetExitHandler(std::function<void()> f)
{
    mExitHandler = f;
}
