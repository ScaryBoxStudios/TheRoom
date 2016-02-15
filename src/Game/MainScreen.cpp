#include "MainScreen.hpp"
#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Graphics/Scene/SceneLoader.hpp"
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"
#include "../Graphics/Scene/SceneFactory.hpp"

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

void MainScreen::onInit(ScreenContext& sc)
{
    // Store engine ref
    mEngine = sc.GetEngine();

    // Store file data cache ref
    mFileDataCache = sc.GetFileDataCache();

    // Cube rotation state
    mRotationData.degreesInc = 0.05f;
    mRotationData.rotating = false;

    // Camera initial position
    mCamera.SetPos(glm::vec3(0, 0, 8));

    // Create world objects
    SetupWorld();

    // Pass the current scene to renderer
    mEngine->GetRenderer().SetScene(mScene.get());
}

void MainScreen::SetupWorld()
{
    // Load sample scene file
    std::string sceneFile= "ext/Scenes/scene.json";
    auto sceneFileData = FileLoad<BufferType>(sceneFile);
    if(!sceneFileData)
        throw std::runtime_error("Couldn't load file (" + sceneFile+ ")");
    SceneLoader sceneLoader;
    SceneFile sf = sceneLoader.Load(*sceneFileData);
    (void) sf;

    SceneFactory factory(
        &mEngine->GetTextureStore(),
        &mEngine->GetRenderer().GetModelStore(),
        &mEngine->GetMaterialStore(),
        mFileDataCache);
    mScene = std::move(factory.CreateFromSceneFile(sf));

    // Set positions for cubes
    SceneNode* node;
    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f,  0.0f,    0.0f),
        glm::vec3( 4.0f, 10.0f,  -20.0f),
        glm::vec3(-3.0f, -4.4f,  - 5.0f),
        glm::vec3(-7.6f, -4.0f,  -14.0f),
        glm::vec3( 4.4f, -3.5f,  - 4.0f),
        glm::vec3(-3.4f,  6.0f,  -15.0f),
        glm::vec3( 2.6f, -4.0f,  -17.0f),
        glm::vec3( 4.0f,  3.0f,  - 5.0f),
        glm::vec3( 3.0f,  0.4f,  -12.0f),
        glm::vec3(-3.5f,  2.0f,  - 3.0f)
    };
    for(std::size_t i = 0; i < cubePositions.size(); ++i)
    {
        const auto& pos = cubePositions[i];

        const std::string name = "cube" + std::to_string(i);
        node = mScene->FindNodeByUuid(name);
        mScene->Move(node, pos);
        mScene->Scale(node, glm::vec3(2.0f));
        mScene->Rotate(node, RotationAxis::X, 20.0f * i);
        mScene->Rotate(node, RotationAxis::Y, 7.0f * i);
        mScene->Rotate(node, RotationAxis::Z, 10.0f * i);
    }
}

std::vector<Camera::MoveDirection> MainScreen::CameraMoveDirections()
{
    auto& window = mEngine->GetWindow();
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

std::tuple<float, float> MainScreen::CameraLookOffset()
{
    auto& window = mEngine->GetWindow();
    std::tuple<double, double> curDiff = window.GetCursorDiff();
    return std::make_tuple(
        static_cast<float>(std::get<0>(curDiff)),
        static_cast<float>(std::get<1>(curDiff))
    );
}

void MainScreen::onKey(Key k, KeyAction ka)
{
    auto& renderer = mEngine->GetRenderer();

    if(k == Key::B && ka == KeyAction::Release)
        renderer.ToggleShowAABBs();
    if(k == Key::R && ka == KeyAction::Release)
        mRotationData.rotating = !mRotationData.rotating;
}

void MainScreen::onUpdate(float dt)
{
    (void) dt;
    // Update the core
    mEngine->Update(dt);

    auto& scene = mScene;
    auto& window = mEngine->GetWindow();

    // Update camera euler angles
    if (window.MouseGrabEnabled())
        mCamera.Look(CameraLookOffset());

    // Update camera position
    mCamera.Move(CameraMoveDirections());

    // Update light position
    SceneNode* teapot = scene->FindNodeByUuid("teapot");
    float increase = 0.7f;
    if(window.IsKeyPressed(Key::Kp8))
        scene->Move(teapot, glm::vec3(0.0f, increase, 0.0f));
    if(window.IsKeyPressed(Key::Kp4))
        scene->Move(teapot, glm::vec3(-increase, 0.0f, 0.0f));
    if(window.IsKeyPressed(Key::Kp2))
        scene->Move(teapot, glm::vec3(0.0f, -increase, 0.0f));
    if(window.IsKeyPressed(Key::Kp6))
        scene->Move(teapot, glm::vec3(increase, 0.0f, 0.0f));
    if(window.IsKeyPressed(Key::Kp5))
        scene->Move(teapot, glm::vec3(0.0f, 0.0f, -increase));
    if(window.IsKeyPressed(Key::Kp0))
        scene->Move(teapot, glm::vec3(0.0f, 0.0f, increase));

    // Update cubes' rotations
    if (mRotationData.rotating)
    {
        for(auto& p : scene->GetNodes())
        {
            if (p.first.substr(0, 4) == "cube")
                scene->Rotate(p.first, RotationAxis::Y, mRotationData.degreesInc);
        }
    }
}

void MainScreen::onRender(float interpolation)
{
    // View calculation with camera
    auto& window = mEngine->GetWindow();
    auto lookOffset = window.MouseGrabEnabled() ? CameraLookOffset() : std::make_tuple(0.0f, 0.0f);
    auto iCamState = mCamera.Interpolate(CameraMoveDirections(), lookOffset, interpolation);

    // Create the view matrix and pass it to the renderer
    glm::mat4 view = glm::lookAt(iCamState.position, iCamState.position + iCamState.front, iCamState.up);

    // Render
    mEngine->GetRenderer().SetView(view);
    mEngine->GetRenderer().Render(interpolation);

    // Render sample text
    mEngine->GetTextRenderer().RenderText("ScaryBox Studios", 10, 10, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
}

void MainScreen::onShutdown()
{
}
