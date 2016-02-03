#include "Game.hpp"
#include <algorithm>
#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Graphics/Image/ImageLoader.hpp"
#include "../Graphics/Geometry/ModelLoader.hpp"
#include "../Graphics/Scene/SceneLoader.hpp"
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
    // Initialize the engine instance
    mEngine.Init();

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

    // Create world objects
    SetupWorld();

    // Pass the current scene to renderer
    mEngine.GetRenderer().SetScene(&mScene);

    // Load the skybox
    auto skybox = std::make_unique<Skybox>();
    ImageLoader imLoader;
    skybox->Load(
        {
            { Skybox::Target::Right,  imLoader.LoadFile("ext/Skybox/right.jpg")  },
            { Skybox::Target::Left,   imLoader.LoadFile("ext/Skybox/left.jpg")   },
            { Skybox::Target::Top,    imLoader.LoadFile("ext/Skybox/top.jpg")    },
            { Skybox::Target::Bottom, imLoader.LoadFile("ext/Skybox/bottom.jpg") },
            { Skybox::Target::Back,   imLoader.LoadFile("ext/Skybox/back.jpg")   },
            { Skybox::Target::Front,  imLoader.LoadFile("ext/Skybox/front.jpg")  }
        }
    );
    mEngine.SetSkybox(std::move(skybox));
}

void Game::SetupWorld()
{
    // Load sample scene file
    std::string sceneFile= "ext/scene.json";
    auto sceneFileData = FileLoad<BufferType>(sceneFile);
    if(!sceneFileData)
        throw std::runtime_error("Couldn't load file (" + sceneFile+ ")");
    SceneLoader sceneLoader;
    SceneFile sf = sceneLoader.Load(*sceneFileData);
    (void) sf;

    auto& scene = mScene;
    auto& renderer = mEngine.GetRenderer();

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

    // Add bottom plane
    {
        const auto& initAABB = renderer.GetModelStore()["cube"]->localAABB;
        scene.CreateNode("cube", "plane", SceneNodeCategory::Normal, initAABB);
        scene.Move("plane", glm::vec3(0.0f, -11.0f, -40.0f));
        scene.Scale("plane", glm::vec3(75.0f, 0.1f, 75.0f));
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
    auto& window = mEngine.GetWindow();
    auto& renderer = mEngine.GetRenderer();

    window.SetCloseHandler(mExitHandler);

    // Setup input event handlers
    window.SetMouseButtonPressHandler(
        [&window](MouseButton mb, ButtonAction ba)
        {
            if (mb == MouseButton::Left && ba == ButtonAction::Press)
                window.SetMouseGrabEnabled(true);
        }
    );
    window.SetKeyPressedHandler(
        [this, &window, &renderer](Key k, KeyAction ka)
        {
            // Exit
            if(k == Key::Escape && ka == KeyAction::Release)
                mExitHandler();
            // Ungrab mouse
            if(k == Key::RightControl && ka == KeyAction::Release)
                window.SetMouseGrabEnabled(false);
            if(k == Key::B && ka == KeyAction::Release)
                renderer.ToggleShowAABBs();
            if(k == Key::R && ka == KeyAction::Release)
                mRotationData.rotating = !mRotationData.rotating;
        }
    );
}


void Game::LoadTextures()
{
    // Retrieve the textureStore from the renderer
    auto& textureStore = mEngine.GetRenderer().GetTextureStore();

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
    auto& renderer = mEngine.GetRenderer();
    auto& modelStore = renderer.GetModelStore();
    auto& textureStore = renderer.GetTextureStore();

    // Model loader instance
    ModelLoader modelLoader;

    struct MdlData
    {
        std::string filepath;
        std::string extension;
        std::string name;
        Material& material;
    };

    // Create materials
    Material mahogany;
    mahogany.SetDiffuseTexture(textureStore["mahogany_wood"]->texId);
    mahogany.SetSpecularTexture(textureStore["mahogany_wood_spec"]->texId);

    Material white;
    white.SetDiffuseColor(glm::vec3(0xFF));

    Material house;
    house.SetDiffuseTexture(textureStore["house_diff"]->texId);
    house.SetSpecularTexture(textureStore["house_spec"]->texId);

    Material well;
    well.SetDiffuseTexture(textureStore["well_diff"]->texId);
    well.SetSpecularTexture(textureStore["well_spec"]->texId);

    std::vector<MdlData> models = {
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

        ModelData model = modelLoader.Load(*file, m.extension.c_str());
        if(model.meshes.size() == 0)
            throw std::runtime_error("Couldn't load model (" + m.filepath + ")");

        modelStore.Load(m.name, std::move(model));
        modelStore[m.name]->material = m.material;
    }
}

std::vector<Camera::MoveDirection> Game::CameraMoveDirections()
{
    auto& window = mEngine.GetWindow();
    std::vector<Camera::MoveDirection> mds;
    if(window.IsKeyPressed(Key::W))
        mds.push_back(Camera::MoveDirection::Forward);
    if(window.IsKeyPressed(Key::A))
        mds.push_back(Camera::MoveDirection::Left);
    if(window.IsKeyPressed(Key::S))
        mds.push_back(Camera::MoveDirection::BackWard);
    if(window.IsKeyPressed(Key::D))
        mds.push_back(Camera::MoveDirection::Right);
    return mds;
}

std::tuple<float, float> Game::CameraLookOffset()
{
    auto& window = mEngine.GetWindow();
    std::tuple<double, double> curDiff = window.GetCursorDiff();
    return std::make_tuple(
        static_cast<float>(std::get<0>(curDiff)),
        static_cast<float>(std::get<1>(curDiff))
    );
}

void Game::Update(float dt)
{
    // Update the core
    mEngine.Update(dt);

    auto& scene = mScene;
    auto& window = mEngine.GetWindow();
    // Update camera euler angles
    if (window.MouseGrabEnabled())
        mCamera.Look(CameraLookOffset());

    // Update camera position
    mCamera.Move(CameraMoveDirections());

    // Update light position
    float increase = 0.7f;
    if(window.IsKeyPressed(Key::Kp8))
        scene.Move("teapot", glm::vec3(0.0f, increase, 0.0f));
    if(window.IsKeyPressed(Key::Kp4))
        scene.Move("teapot", glm::vec3(-increase, 0.0f, 0.0f));
    if(window.IsKeyPressed(Key::Kp2))
        scene.Move("teapot", glm::vec3(0.0f, -increase, 0.0f));
    if(window.IsKeyPressed(Key::Kp6))
        scene.Move("teapot", glm::vec3(increase, 0.0f, 0.0f));
    if(window.IsKeyPressed(Key::Kp5))
        scene.Move("teapot", glm::vec3(0.0f, 0.0f, -increase));
    if(window.IsKeyPressed(Key::Kp0))
        scene.Move("teapot", glm::vec3(0.0f, 0.0f, increase));

    // Update cubes' rotations
    if (mRotationData.rotating)
    {
        for(auto& p : scene.GetNodes())
        {
            if (p.first.substr(0, 4) == "cube")
                scene.Rotate(p.first, RotationAxis::Y, mRotationData.degreesInc);
        }
    }
}

void Game::Render(float interpolation)
{
    // View calculation with camera
    auto& window = mEngine.GetWindow();
    auto lookOffset = window.MouseGrabEnabled() ? CameraLookOffset() : std::make_tuple(0.0f, 0.0f);
    auto iCamState = mCamera.Interpolate(CameraMoveDirections(), lookOffset, interpolation);

    // Create the view matrix and pass it to the renderer
    glm::mat4 view = glm::lookAt(iCamState.position, iCamState.position + iCamState.front, iCamState.up);

    // Render
    mEngine.GetRenderer().SetView(view);
    mEngine.Render(interpolation);
}

void Game::Shutdown()
{
    mEngine.Shutdown();
}

void Game::SetExitHandler(std::function<void()> f)
{
    mExitHandler = f;
}
