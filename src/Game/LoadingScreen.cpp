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
    mEngine->GetTextRenderer().GetFontStore().LoadFont("visitor", "ext/Assets/Fonts/visitor.ttf");

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

void LoadingScreen::LoadFileData()
{
    std::vector<std::string> fileList =
    {
      // Skybox
      "ext/Assets/Textures/Skybox/Bluesky/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/front.jpg"
    // Irradiance map
    , "ext/Assets/Textures/Skybox/Bluesky/Irradiance/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Irradiance/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Irradiance/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Irradiance/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Irradiance/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Irradiance/front.jpg"
    // Radiance map
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/0/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/0/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/0/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/0/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/0/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/0/front.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/1/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/1/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/1/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/1/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/1/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/1/front.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/2/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/2/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/2/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/2/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/2/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/2/front.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/3/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/3/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/3/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/3/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/3/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/3/front.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/4/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/4/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/4/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/4/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/4/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/4/front.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/5/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/5/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/5/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/5/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/5/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/5/front.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/6/right.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/6/left.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/6/top.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/6/bottom.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/6/back.jpg"
    , "ext/Assets/Textures/Skybox/Bluesky/Radiance/6/front.jpg"

      // Skysphere
    , "ext/Assets/Textures/Skysphere/Day Sun Peak Summersky.png"
      // Textures
    , "ext/Assets/Textures/Materials/Mahogany/mahogany_wood.jpg"
    , "ext/Assets/Textures/Materials/Mahogany/mahogany_wood_spec.jpg"
    , "ext/Assets/Textures/Models/WoodenCabin/WoodCabinDif.jpg"
    , "ext/Assets/Textures/Models/WoodenCabin/WoodCabinSM.jpg"
    , "ext/Assets/Textures/Models/WoodenCabin/WoodCabinNM.jpg"
    , "ext/Assets/Textures/Materials/Stone/brickwall.jpg"
    , "ext/Assets/Textures/Materials/Stone/brickwall_NM.jpg"
    , "ext/Assets/Textures/Models/Well/Wall1_T.tga"
    , "ext/Assets/Textures/Models/Well/Wall1_B.tga"
    , "ext/Assets/Textures/Models/Warrior/cuirassplate_B.tga"
    , "ext/Assets/Textures/Models/Warrior/cuirassplate_N.tga"
    , "ext/Assets/Textures/Models/Warrior/cuirassplate_T.tga"
    , "ext/Assets/Textures/Models/Warrior/gauntletsplate_B.tga"
    , "ext/Assets/Textures/Models/Warrior/gauntletsplate_N.tga"
    , "ext/Assets/Textures/Models/Warrior/gauntletsplate_T.tga"
    , "ext/Assets/Textures/Models/Warrior/helmetplate01_B.tga"
    , "ext/Assets/Textures/Models/Warrior/helmetplate01_N.tga"
    , "ext/Assets/Textures/Models/Warrior/helmetplate01_T.tga"
    , "ext/Assets/Textures/Models/Barrel/barrel2.tif"
    , "ext/Assets/Textures/Models/Barrel/barrel2_nmp.tif"
      // Models
    , "ext/Assets/Models/Cube.obj"
    , "ext/Assets/Models/Teapot.obj"
    , "ext/Assets/Models/WoodenCabin.dae"
    , "ext/Assets/Models/Well.obj"
    , "ext/Assets/Models/Warrior.dae"
    , "ext/Assets/Models/Barrel2.fbx"
    , "ext/Assets/Models/ShaderBall.fbx"
    };

    for (const auto& file : fileList)
    {
        mCurrentlyLoading = file;
        mFileDataCache->emplace(file, FileLoad<BufferType>(file));
        if(!(*mFileDataCache)[file])
            throw std::runtime_error("Couldn't load file (" + file + ")");
    }
};

void LoadingScreen::onUpdate(float dt)
{
    mEngine->Update(dt);

    if (mFileCacheIsReady)
    {
        mOnLoadedCb();
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

void LoadingScreen::SetOnLoadedCb(OnLoadedCb cb)
{
    mOnLoadedCb = cb;
}
