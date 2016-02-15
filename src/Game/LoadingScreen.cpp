#include "LoadingScreen.hpp"
#include <thread>
WARN_GUARD_ON
#include "../Graphics/Image/ImageLoader.hpp"
#include "../Graphics/Geometry/ModelLoader.hpp"
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"

void LoadingScreen::onInit(ScreenContext& sc)
{
    // Store the engine reference
    mEngine = sc.GetEngine();

    // Store the file data cache reference
    mFileDataCache = sc.GetFileDataCache();

    // Load font
    mEngine->GetTextRenderer().GetFontStore().LoadFont("visitor", "ext/Assets/visitor.ttf");

    // Fire loader thread
    mFileCacheIsReady = false;
    std::thread loaderThread(
        [this]()
        {
            LoadFileData();
            mFileCacheIsReady = true;
        }
    );
    loaderThread.detach();
}

void LoadingScreen::LoadFromMem()
{
    // Load the textures
    LoadTextures();

    // Load the materials
    LoadMaterials();

    // Load the models
    LoadModels();

    // Load the skybox
    auto skybox = std::make_unique<Skybox>();
    ImageLoader imLoader;
    skybox->Load(
        {
            { Skybox::Target::Right,  imLoader.Load(*(*mFileDataCache)["ext/Assets/Skybox/right.jpg"], "jpg") },
            { Skybox::Target::Left,   imLoader.Load(*(*mFileDataCache)["ext/Assets/Skybox/left.jpg"],  "jpg") },
            { Skybox::Target::Top,    imLoader.Load(*(*mFileDataCache)["ext/Assets/Skybox/top.jpg"],   "jpg") },
            { Skybox::Target::Bottom, imLoader.Load(*(*mFileDataCache)["ext/Assets/Skybox/bottom.jpg"],"jpg") },
            { Skybox::Target::Back,   imLoader.Load(*(*mFileDataCache)["ext/Assets/Skybox/back.jpg"],  "jpg") },
            { Skybox::Target::Front,  imLoader.Load(*(*mFileDataCache)["ext/Assets/Skybox/front.jpg"], "jpg") }
        }
    );
    mEngine->SetSkybox(std::move(skybox));

    // Call finish
    mFinishCb();
}

void LoadingScreen::LoadFileData()
{
    std::vector<std::string> fileList =
    {
      // Skybox
      "ext/Assets/Skybox/right.jpg"
    , "ext/Assets/Skybox/left.jpg"
    , "ext/Assets/Skybox/top.jpg"
    , "ext/Assets/Skybox/bottom.jpg"
    , "ext/Assets/Skybox/back.jpg"
    , "ext/Assets/Skybox/front.jpg"
      // Textures
    , "ext/Assets/mahogany_wood.jpg"
    , "ext/Assets/mahogany_wood_spec.jpg"
    , "ext/Assets/WoodenCabin/WoodCabinDif.jpg"
    , "ext/Assets/WoodenCabin/WoodCabinSM.jpg"
    , "ext/Assets/WoodenCabin/WoodCabinNM.jpg"
    , "ext/Assets/brickwall.jpg"
    , "ext/Assets/brickwall_NM.jpg"
    , "ext/Assets/Dungeon/maps/Wall1_T.tga"
    , "ext/Assets/Dungeon/maps/Wall1_B.tga"
      // Models
    , "ext/Assets/Cube/cube.obj"
    , "ext/Assets/teapot.obj"
    , "ext/Assets/WoodenCabin/WoodenCabin.dae"
    , "ext/Assets/Dungeon/Well.obj"
    };

    for (const auto& file : fileList)
    {
        mCurrentlyLoading = file;
        mFileDataCache->emplace(file, FileLoad<BufferType>(file));
        if(!(*mFileDataCache)[file])
            throw std::runtime_error("Couldn't load file (" + file + ")");
    }
};

void LoadingScreen::LoadTextures()
{
}

void LoadingScreen::LoadMaterials()
{   
}

void LoadingScreen::LoadModels()
{
    // Retrieve the model and texture stores from the renderer
    auto& renderer      = mEngine->GetRenderer();
    auto& modelStore    = renderer.GetModelStore();

    // Add sample UV Sphere
    ModelData sphereModel = GenUVSphere(1, 32, 32);
    modelStore.Load("63a3c6df-78b7-4453-9129-2ce5b2eaf620", std::move(sphereModel));
}

void LoadingScreen::onUpdate(float dt)
{
    mEngine->Update(dt);

    if (mFileCacheIsReady)
    {
        LoadFromMem();
        mFileCacheIsReady = false;
    }
}

void LoadingScreen::onRender(float interpolation)
{
    (void) interpolation;
    // Clear
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Show indicator
    std::string indicator = "Loading ";
    indicator += mCurrentlyLoading;
    indicator += "...";
    mEngine->GetTextRenderer().RenderText(indicator, 10, 10, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
}

void LoadingScreen::onShutdown()
{
}

void LoadingScreen::SetFinishCb(FinishCb cb)
{
    mFinishCb = cb;
}
