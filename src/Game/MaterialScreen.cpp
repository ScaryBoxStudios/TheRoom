#include "MaterialScreen.hpp"
#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Graphics/Scene/SceneLoader.hpp"
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"
#include "../Graphics/Image/ImageLoader.hpp"
#include "../Graphics/Scene/SceneFactory.hpp"

void MaterialScreen::onInit(ScreenContext& sc)
{
    // Store engine ref
    mEngine = sc.GetEngine();

    // Camera initial position
    mCamera.SetPos(glm::vec3(0, 0, 8));

    // Clear previous stores
    mEngine->GetModelStore().Clear();
    mEngine->GetMaterialStore().Clear();
    mEngine->GetTextureStore().Clear();

    // Load sample scene file
    std::string sceneFile= "ext/Scenes/scene2.json";
    auto sceneFileData = FileLoad<BufferType>(sceneFile);
    if(!sceneFileData)
        throw std::runtime_error("Couldn't load file (" + sceneFile + ")");
    SceneLoader sceneLoader;
    SceneFile sf = sceneLoader.Load(*sceneFileData);

    // Add sample UV Sphere
    ModelData sphereModel = GenUVSphere(1, 32, 32);
    mEngine->GetModelStore().Load("2", std::move(sphereModel));

    auto fileDataCache = sc.GetFileDataCache();
    SceneFactory factory(
        &mEngine->GetTextureStore(),
        &mEngine->GetModelStore(),
        &mEngine->GetMaterialStore(),
        fileDataCache);
    mScene = factory.CreateFromSceneFile(sf);

    // Pass the current scene to renderer
    mEngine->GetRenderer().SetScene(mScene.get());

    // Initialize skysphere
    ImageLoader imLoader;
    mSkysphere = std::make_unique<Skysphere>();
    mSkysphere->Load(
        imLoader.Load(*(*fileDataCache)["ext/Assets/Textures/Skysphere/Day Sun Peak Summersky.png"], "png"));
}

std::vector<Camera::MoveDirection> MaterialScreen::CameraMoveDirections()
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

std::tuple<float, float> MaterialScreen::CameraLookOffset()
{
    auto& window = mEngine->GetWindow();
    std::tuple<double, double> curDiff = window.GetCursorDiff();
    return std::make_tuple(
        static_cast<float>(std::get<0>(curDiff)),
        static_cast<float>(std::get<1>(curDiff))
    );
}

void MaterialScreen::onKey(Key k, KeyAction ka)
{
    (void)k;
    (void)ka;
}

void MaterialScreen::onUpdate(float dt)
{
    (void) dt;
    // Update the core
    mEngine->Update(dt);

    //
    auto& window = mEngine->GetWindow();

    // Update interpolation variables
    for (auto& obj : mScene->GetNodes())
    {
        Transform& trans = obj.second->GetTransformation();
        trans.Update();

        AABB& aabb = obj.second->GetAABB();
        aabb.Update(trans.GetPosition(), trans.GetScale(), trans.GetRotation());
    }

    // Update camera euler angles
    if (window.MouseGrabEnabled())
        mCamera.Look(CameraLookOffset());

    // Update camera position
    mCamera.Move(CameraMoveDirections());

    // Update the camera matrix
    mCamera.Update();
}

void MaterialScreen::onRender(float interpolation)
{
    // Get the view matrix and pass it to the renderer
    glm::mat4 view = mCamera.InterpolatedView(interpolation);

    // Render
    mEngine->GetRenderer().SetView(view);
    mEngine->GetRenderer().Render(interpolation);

    // Render skysphere
    mSkysphere->Render(mEngine->GetRenderer().GetProjection(), view);

    // Render sample text
    mEngine->GetTextRenderer().RenderText("ScaryBox Studios", 10, 10, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
}

void MaterialScreen::onShutdown()
{
}
