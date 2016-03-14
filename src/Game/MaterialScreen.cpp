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
    mCamera.SetPos(glm::vec3(-2, 2, 10));
    mCamera.Look(std::make_tuple(10/0.05f, 10/0.05f));

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

    // Setup scene lights
    Lights& lights = mEngine->GetRenderer().GetLights();

    // Add directional light
    DirLight dirLight;
    dirLight.direction = glm::vec3(-0.3f, -0.5f, -0.5f);
    dirLight.properties.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    dirLight.properties.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    dirLight.properties.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    lights.dirLights.push_back(dirLight);

    // Initialize skysphere
    ImageLoader imLoader;
    mSkysphere = std::make_unique<Skysphere>();
    mSkysphere->Load(
        imLoader.Load(*(*fileDataCache)["ext/Assets/Textures/Skysphere/Day Sun Peak Summersky.png"], "png"));

    // Init renderform creator
    mRenderformCreator = std::make_unique<RenderformCreator>(&(mEngine->GetModelStore()), &(mEngine->GetMaterialStore()));
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
    if(k == Key::F1 && ka == KeyAction::Release)
        mOnNextScreenCb();
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

    // Update render form
    mRenderformCreator->Update(mScene->PullUpdates());

    auto bakeIntForm = [interpolation, this]() -> Renderer::IntForm
    {
        Renderer::IntForm rVal;
        RenderformCreator::Renderform form = mRenderformCreator->GetRenderform();

        for (const auto& p : form)
        {
            // Create new element
            Renderer::IntMaterial im;
            std::vector<Renderer::IntMesh> vec;
            rVal.push_back({ im, vec });

            // Retrieve newly added element
            auto& newEntry = rVal[rVal.size() - 1];

            const auto& rformMat    = p.second;
            const auto& rformMeshes = p.second.meshes;
            std::vector<Renderer::IntMesh>& meshes = newEntry.second;

            newEntry.first =
            { rformMat.diffTexId
            , rformMat.specTexId
            , rformMat.nmapTexId
            , rformMat.matIndex
            , rformMat.useNormalMap
            };

            for (const auto& rformMesh : rformMeshes)
            {
                Renderer::IntMesh newMesh;
                newMesh.transformation = *rformMesh.transformation;
                newMesh.vaoId      = rformMesh.vaoId;
                newMesh.eboId      = rformMesh.eboId;
                newMesh.numIndices = rformMesh.numIndices;
                meshes.push_back(newMesh);
            }
        }

        return rVal;
    };

    // Render
    mEngine->GetRenderer().SetView(view);
    mEngine->GetRenderer().Render(interpolation, bakeIntForm());

    // Render skysphere
    mSkysphere->Render(mEngine->GetRenderer().GetProjection(), view);

    // Render sample text
    mEngine->GetTextRenderer().RenderText("ScaryBox Studios", 10, 10, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
}

void MaterialScreen::onShutdown()
{
    mEngine->GetModelStore().Clear();
    mEngine->GetMaterialStore().Clear();
    mEngine->GetTextureStore().Clear();
}

void MaterialScreen::SetOnNextScreenCb(OnNextScreenCb cb)
{
    mOnNextScreenCb = cb;
}
