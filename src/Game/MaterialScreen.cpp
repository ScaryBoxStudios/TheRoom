#include "MaterialScreen.hpp"
#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Asset/Scene/SceneLoader.hpp"
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"
#include "../Asset/Image/ImageLoader.hpp"
#include "../Graphics/Scene/SceneFactory.hpp"

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

void MaterialScreen::onInit(ScreenContext& sc)
{
    // Store engine ref
    mEngine = sc.GetEngine();

    // Store file data cache ref
    mFileDataCache = sc.GetFileDataCache();

    // Add sample UV Sphere
    ModelData sphereModel = GenUVSphere(1, 32, 32);
    mEngine->GetModelStore().Load("sphere", std::move(sphereModel));

    // Create scene
    mScene = std::make_unique<Scene>();

    // Create materials
    int id = 0;
    for (int metallic = 0; metallic <= 1; ++metallic)
    {
        for (float roughness = 0.0f; roughness < 1.0f; roughness += 0.1f)
        {
            for (float reflectivity = 0.0f; reflectivity < 1.0; reflectivity += 0.1f)
            {
                // Create and store the material
                std::string materialName;
                materialName += metallic == 1 ? "m" : "d";
                materialName += "r" + std::to_string(roughness);
                materialName += "f" + std::to_string(reflectivity);
                Material m;
                m.SetMetallic(metallic);
                m.SetRoughness(roughness);
                m.SetFresnel(reflectivity);
                m.SetDiffuseColor(glm::vec3(255.0f, 0.0f, 0.0f));
                mEngine->GetMaterialStore().Load(materialName, m);

                // Create and store the object that will be made of the material previously defined
                SceneNode* node = mScene->CreateNode(
                    "sphere",
                    {materialName},
                    std::to_string(id),
                    Category::Normal,
                    sphereModel.boundingBox
                );
                node->Move(glm::vec3(roughness * 20.0f -10.0f + 22.0f * metallic, reflectivity * 20.0f - 10.0f, 0.0f));
                ++id;
            }
        }
    }

    // Setup scene lights
    Lights& lights = mEngine->GetRenderer().GetLights();

    // Add directional light
    DirLight dirLight;
    dirLight.direction = glm::vec3(-0.3f, -0.5f, -0.5f);
    dirLight.color = glm::vec3(0.9f);
    lights.dirLights.push_back(dirLight);

    // Camera initial position
    mCamera.SetPos(glm::vec3(-6, 8, 12));
    mCamera.Look(std::make_tuple(450.0f, 450.0f));

    // Load the skybox
    ImageLoader imLoader;

    mSkybox = std::make_unique<Skybox>();
    mSkybox->Load(imLoader.Load(*(*mFileDataCache)["ext/Assets/Textures/Skybox/Indoors/indoors.tga"], "tga"));

    // Load the irr map
    mIrrMap = std::make_unique<Cubemap>();
    mIrrMap->SetData(imLoader.Load(*(*mFileDataCache)["ext/Assets/Textures/Skybox/Indoors/indoors_irr.tga"], "tga"));

    // Load the rad map
    mRadMap = std::make_unique<Cubemap>();
    for (unsigned int i = 0; i < 9; ++i) {
        mRadMap->SetData(
            imLoader.Load(*(*mFileDataCache)[
                "ext/Assets/Textures/Skybox/Indoors/indoors_rad_" + std::to_string(i) + ".tga"], "tga"), i);
    }

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
}

void MaterialScreen::onUpdate(float dt)
{
    // Aliases
    auto& window = mEngine->GetWindow();

    // Update the core
    mEngine->Update(dt);

    // Update camera euler angles
    if (window.MouseGrabEnabled())
        mCamera.Look(CameraLookOffset());

    // Update camera position
    mCamera.Move(CameraMoveDirections());

    // Update interpolation variables
    for (auto& obj : mScene->GetNodes())
    {
        Transform& trans = obj.second->GetTransformation();
        trans.Update();

        AABB& aabb = obj.second->GetAABB();
        aabb.Update(trans.GetPosition(), trans.GetScale(), trans.GetRotation());
    }

    // Update the camera matrix
    mCamera.Update();
}

void MaterialScreen::onRender(float interpolation)
{
    // Get the view matrix and pass it to the renderer
    glm::mat4 view = mCamera.InterpolatedView(interpolation);

    // Update render form
    mRenderformCreator->Update(mScene->PullUpdates());

    // Convert render form to int form
    auto intForm = bakeIntForm(*mRenderformCreator);

    // Add skybox and irrMap id to intform
    intForm.skyboxId = mSkybox->GetCubemap()->Id();
    intForm.irrMapId = mIrrMap->Id();
    intForm.radMapId = mRadMap->Id();

    // Render
    mEngine->GetRenderer().SetView(view);
    mEngine->GetRenderer().Render(interpolation, intForm);

    // Render the skybox
    if (mSkybox)
        mSkybox->Render(mEngine->GetRenderer().GetProjection(), view);

    // Render sample text
    mEngine->GetTextRenderer().RenderText("ScaryBox Studios", 10, 10, 32, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
}

void MaterialScreen::onShutdown()
{
    // Remove lights
    mEngine->GetRenderer().GetLights().pointLights.clear();
    mEngine->GetRenderer().GetLights().dirLights.clear();

    // Clear previous stores
    mEngine->GetModelStore().Clear();
    mEngine->GetMaterialStore().Clear();
    mEngine->GetTextureStore().Clear();
}
