#include "GalleryScreen.hpp"
#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"
#include "../Asset/Image/ImageLoader.hpp"
#include "../Asset/Properties/Properties.hpp"
#include "../Asset/Properties/PropertiesManager.hpp"
#include "../Graphics/Scene/SceneFactory.hpp"

// Skybox, Irrmap and Radmap names for cubemap store
const std::string skybox = "gallery_skybox";
const std::string irrmap = "gallery_irr";
const std::string radmap = "gallery_rad";

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

void GalleryScreen::onInit(ScreenContext& sc)
{
    // Store engine ref
    mEngine = sc.GetEngine();

    // Store file data cache ref
    mFileDataCache = sc.GetFileDataCache();

    PropertiesManager propMgr;
    Properties::SceneFile scene = propMgr.Load
        // Scenes
        ( {{ "galleryscene",  "res/Properties/Scenes/gallery.scn" }}
        // Materials
        , {{ "bronze",        "res/Properties/Materials/bronze.mat"        }
          ,{ "brown_plastic", "res/Properties/Materials/brown_plastic.mat" }
          ,{ "concrete",      "res/Properties/Materials/concrete.mat"      }
          ,{ "mahogany",      "res/Properties/Materials/mahogany.mat"      }
          ,{ "mirror",        "res/Properties/Materials/mirror.mat"        }
          ,{ "stone",         "res/Properties/Materials/stone.mat"         }
          ,{ "white",         "res/Properties/Materials/white.mat"         }}
        // Models
        , {{ "cube",       "res/Properties/Models/cube.mod"       }
          ,{ "teapot",     "res/Properties/Models/teapot.mod"     }
          ,{ "shaderball", "res/Properties/Models/shaderball.mod" }}
        );

    SceneFactory factory(
        &mEngine->GetTextureStore(),
        &mEngine->GetModelStore(),
        &mEngine->GetMaterialStore(),
        mFileDataCache);
    mScene = factory.CreateFromSceneFile(scene);

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
    auto& cubemapStore = mEngine->GetCubemapStore();
    cubemapStore.Load(skybox, imLoader.Load(*(*mFileDataCache)["ext/Assets/Textures/Skybox/Bluesky/bluesky.tga"], "tga"));
    mEngine->GetSkyboxRenderer().SetCubemapId(cubemapStore[skybox]->id);

    // Load the irr map
    mEngine->GetCubemapStore().Load(irrmap, imLoader.Load(*(*mFileDataCache)["ext/Assets/Textures/Skybox/Bluesky/bluesky_irr.tga"], "tga"));

    // Load the rad map
    for (unsigned int i = 0; i < 9; ++i) {
        mEngine->GetCubemapStore().Load(
            radmap,
            imLoader.Load(*(*mFileDataCache)[
                "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_" + std::to_string(i) + ".tga"], "tga"), i);
    }

    // Init renderform creator
    mRenderformCreator = std::make_unique<RenderformCreator>(&(mEngine->GetModelStore()), &(mEngine->GetMaterialStore()));
}

std::vector<Camera::MoveDirection> GalleryScreen::CameraMoveDirections()
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

std::tuple<float, float> GalleryScreen::CameraLookOffset()
{
    auto& window = mEngine->GetWindow();
    std::tuple<double, double> curDiff = window.GetCursorDiff();
    return std::make_tuple(
        static_cast<float>(std::get<0>(curDiff)),
        static_cast<float>(std::get<1>(curDiff))
    );
}

void GalleryScreen::onKey(Key k, KeyAction ka)
{
}

void GalleryScreen::onUpdate(float dt)
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

void GalleryScreen::onRender(float interpolation)
{
    auto& renderer       = mEngine->GetRenderer();
    auto& skyboxRenderer = mEngine->GetSkyboxRenderer();
    auto& cubemapStore   = mEngine->GetCubemapStore();

    // Get the view matrix and pass it to the renderer
    glm::mat4 view = mCamera.InterpolatedView(interpolation);

    // Update render form
    mRenderformCreator->Update(mScene->PullUpdates());

    // Convert render form to int form
    auto intForm = bakeIntForm(*mRenderformCreator);

    // Add skybox and irrMap id to intform
    intForm.skyboxId = cubemapStore[skybox]->id;
    intForm.irrMapId = cubemapStore[irrmap]->id;
    intForm.radMapId = cubemapStore[radmap]->id;

    // Render
    renderer.SetView(view);
    renderer.Render(interpolation, intForm);

    // Render the skybox
    skyboxRenderer.Render(mEngine->GetRenderer().GetProjection(), view);

    // Render sample text
    mEngine->GetTextRenderer().RenderText("ScaryBox Studios", 10, 10, 32, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
}

void GalleryScreen::onShutdown()
{
    // Remove lights
    mEngine->GetRenderer().GetLights().pointLights.clear();
    mEngine->GetRenderer().GetLights().dirLights.clear();

    // Clear previous stores
    mEngine->GetModelStore().Clear();
    mEngine->GetMaterialStore().Clear();
    mEngine->GetTextureStore().Clear();
    mEngine->GetCubemapStore().Clear();
}
