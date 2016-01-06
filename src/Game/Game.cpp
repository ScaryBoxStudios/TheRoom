#include "Game.hpp"
#include <algorithm>
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

    // Initialize the renderer
    renderer.Init(
        mWindow.GetWidth(),
        mWindow.GetHeight(),
        mShaderPrograms.at("geometry_pass").Id(),
        mShaderPrograms.at("light_pass").Id()
    );

    // Pass the current scene to renderer
    renderer.SetScene(&mScene);

    // Load the skybox
    mSkybox = std::make_unique<Skybox>();
    ImageLoader imLoader;
    mSkybox->Load(
        {
            { Skybox::Target::Right,  imLoader.LoadFile("ext/Skybox/right.jpg")  },
            { Skybox::Target::Left,   imLoader.LoadFile("ext/Skybox/left.jpg")   },
            { Skybox::Target::Top,    imLoader.LoadFile("ext/Skybox/top.jpg")    },
            { Skybox::Target::Bottom, imLoader.LoadFile("ext/Skybox/bottom.jpg") },
            { Skybox::Target::Back,   imLoader.LoadFile("ext/Skybox/back.jpg")   },
            { Skybox::Target::Front,  imLoader.LoadFile("ext/Skybox/front.jpg")  }
        }
    );
    renderer.SetSkybox(mSkybox.get());
}

void Game::SetupWorld()
{
    auto& scene = mScene;

    //
    // Normal objects
    //
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

        const std::string name = "cube" + std::to_string(i);
        const auto& initAABB = renderer.GetModelStore()["cube"]->localAABB;
        scene.CreateNode("cube", name, SceneNodeCategory::Normal, initAABB);
        scene.Move(name, pos);
        scene.Scale(name, glm::vec3(2.0f));
        scene.Rotate(name, RotationAxis::X, 20.0f * i);
        scene.Rotate(name, RotationAxis::Y, 7.0f * i);
        scene.Rotate(name, RotationAxis::Z, 10.0f * i);
    }

    // Add house
    {
        const auto& initAABB = renderer.GetModelStore()["house"]->localAABB;
        scene.CreateNode("house", "house", SceneNodeCategory::Normal, initAABB);
        scene.Move("house", glm::vec3(0.0f, -10.0f, -40.0f));
        scene.Scale("house", glm::vec3(0.3f));
    }

    // Add well
    {
        const auto& initAABB = renderer.GetModelStore()["well"]->localAABB;
        scene.CreateNode("well", "well", SceneNodeCategory::Normal, initAABB);
        scene.Move("well", glm::vec3(0.0f, -5.0f, -10.0f));
        scene.Scale("well", glm::vec3(2.0f));
    }

    //
    // Light objects
    //
    // Add cube lights
    {
        const auto& initAABB = renderer.GetModelStore()["teapot"]->localAABB;
        scene.CreateNode("teapot", "teapot", SceneNodeCategory::Light, initAABB);
        scene.Move("teapot", glm::vec3(4.0f, 0.0f, 0.0f));
        scene.Scale("teapot", glm::vec3(0.3f));
    }
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
            if(k == Key::B && ka == KeyAction::Release)
                renderer.ToggleShowAABBs();
            if(k == Key::R && ka == KeyAction::Release)
                mRotationData.rotating = !mRotationData.rotating;
        }
    );
}

void Game::LoadShaders()
{
    // The shader map
    std::unordered_map<std::string, std::vector<std::pair<Shader::Type, std::string>>> shadersLoc =
    {
        {
            "geometry_pass",
            {
                { Shader::Type::Vertex,   "res/geometry_pass_vert.glsl" },
                { Shader::Type::Fragment, "res/geometry_pass_frag.glsl" },
            }
        },
        {
            "light_pass",
            {
                { Shader::Type::Vertex,   "res/light_pass_vert.glsl" },
                { Shader::Type::Fragment, "res/light_pass_frag.glsl" },
            }
        }
    };

    // Load shaders
    for (const auto& p : shadersLoc)
    {
        // Will temporarily store the the compiled shaders
        std::unordered_map<Shader::Type, Shader> shaders;
        for (const auto& f : p.second)
        {
            // Load file
            auto shaderFile = FileLoad<BufferType>(f.second);
            if (!shaderFile)
                throw std::runtime_error("Could not find shader file: \n" + f.second);

            // Convert it to std::string containter
            std::string shaderSrc((*shaderFile).begin(), (*shaderFile).end());

            // Create shader from source
            Shader shader(shaderSrc, f.first);

            // Insert loaded shader id to temp map
            shaders.emplace(f.first, std::move(shader));
        }

        // Fetch vert and frag shader id's from temp map and link
        ShaderProgram program(
            shaders.at(Shader::Type::Vertex).Id(),
            shaders.at(Shader::Type::Fragment).Id()
        );
        mShaderPrograms.emplace(p.first, std::move(program));
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
        {"ext/Dungeon/maps/Wall1_T.tga",     "well_diff"},
        {"ext/Dungeon/maps/Wall1_B.tga",     "well_spec"}
    };

    // Load the textures
    for (const auto& p : textures)
    {
        RawImage<> pb = imageLoader.LoadFile(p.first);
        textureStore.Load(p.second, pb);
    }
}

void Game::LoadModels()
{
    // Retrieve the model and texture stores from the renderer
    auto& modelStore = renderer.GetModelStore();
    auto& textureStore = renderer.GetTextureStore();

    // Model loader instance
    ModelLoader modelLoader;

    struct ModelData
    {
        std::string filepath;
        std::string extension;
        std::string name;
        Material& material;
    };

    // Create materials
    Material mahogany;
    mahogany.diffuseTexId = textureStore["mahogany_wood"]->texId;
    mahogany.specularTexId = textureStore["mahogany_wood_spec"]->texId;
    mahogany.usesDiffTex = true;
    mahogany.usesSpecTex = true;

    Material white;
    white.diffuseColor = glm::vec3(0xFF);

    Material house;
    house.diffuseTexId = textureStore["house_diff"]->texId;
    house.specularTexId = textureStore["house_spec"]->texId;
    house.usesDiffTex = true;
    house.usesSpecTex = true;

    Material well;
    well.diffuseTexId = textureStore["well_diff"]->texId;
    well.specularTexId = textureStore["well_spec"]->texId;
    well.usesDiffTex = true;
    well.usesSpecTex = true;

    std::vector<ModelData> models = {
        { "ext/Cube/cube.obj",               "obj", "cube",   mahogany } // Cube
    ,   { "ext/teapot.obj",                  "obj", "teapot", white    } // Teapot
    ,   { "ext/WoodenCabin/WoodenCabin.dae", "dae", "house",  house    } // House
    ,   { "ext/Dungeon/Well.obj",            "obj", "well",   well     } // Well
    };

    for(auto& m : models)
    {
        auto file = FileLoad<BufferType>(m.filepath);
        if(!file)
            throw std::runtime_error("Couldn't load file (" + m.filepath + ")");

        Model model = modelLoader.Load(*file, m.extension.c_str());
        if(model.meshes.size() == 0)
            throw std::runtime_error("Couldn't load model (" + m.filepath + ")");

        modelStore.Load(m.name, std::move(model));
        modelStore[m.name]->material = m.material;
    }
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
    auto& scene = mScene;
    float increase = 0.7f;
    if(mWindow.IsKeyPressed(Key::Kp8))
        scene.Move("teapot", glm::vec3(0.0f, increase, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp4))
        scene.Move("teapot", glm::vec3(-increase, 0.0f, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp2))
        scene.Move("teapot", glm::vec3(0.0f, -increase, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp6))
        scene.Move("teapot", glm::vec3(increase, 0.0f, 0.0f));
    if(mWindow.IsKeyPressed(Key::Kp5))
        scene.Move("teapot", glm::vec3(0.0f, 0.0f, -increase));
    if(mWindow.IsKeyPressed(Key::Kp0))
        scene.Move("teapot", glm::vec3(0.0f, 0.0f, increase));

    // Update cubes' rotations
    if (mRotationData.rotating)
    {
        for(auto& p : scene.GetNodes())
        {
            switch(p.second->GetCategory())
            {
                case SceneNodeCategory::Normal:
                    scene.Rotate(p.first, RotationAxis::Y, mRotationData.degreesInc);
                    break;
            }
        }
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
    // Destroy Skybox
    mSkybox.reset();

    // Renderer
    renderer.Shutdown();

    // Window
    mWindow.Destroy();
}

void Game::SetExitHandler(std::function<void()> f)
{
    mExitHandler = f;
}
