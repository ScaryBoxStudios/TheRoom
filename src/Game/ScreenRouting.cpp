#include "ScreenRouting.hpp"
#include <vector>
#include <string>
#include "LoadingScreen.hpp"
#include "MainScreen.hpp"
#include "GalleryScreen.hpp"
#include "MaterialScreen.hpp"

#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
const std::vector<std::string> mainScrFileList = {
    // Skysphere
      "ext/Assets/Textures/Skysphere/Day Sun Peak Summersky.png"
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

const std::vector<std::string> galleryScrFileList = {
      // Textures
      "ext/Assets/Textures/Materials/Mahogany/mahogany_wood.jpg"
    , "ext/Assets/Textures/Materials/Mahogany/mahogany_wood_spec.jpg"
    , "ext/Assets/Textures/Materials/Stone/brickwall.jpg"
    , "ext/Assets/Textures/Materials/Stone/brickwall_NM.jpg"
    , "ext/Assets/Textures/Models/Room/ConcreteDiff.png"
    , "ext/Assets/Textures/Models/Room/ConcreteNrm.png"
    , "ext/Assets/Textures/Models/Room/FloorDiff.png"
    , "ext/Assets/Textures/Models/Room/FloorNrm.png"
    , "ext/Assets/Textures/Models/Room/MarbleTex.png"
      // Models
    , "ext/Assets/Models/Cube.obj"
    , "ext/Assets/Models/Teapot.obj"
    , "ext/Assets/Models/ShaderBall.fbx"
    , "ext/Assets/Models/Room.fbx"
};

const std::vector<std::string> blueSkyEnvMap = {
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
};

const std::vector<std::string> indoorsEnvMap = {
      "ext/Assets/Textures/Skybox/Indoors/indoors.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_irr.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_0.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_1.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_2.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_3.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_4.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_5.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_6.tga"
};
WARN_GUARD_OFF

ScreenRouter::ScreenRouter(ScreenContext context)
  : mScrContext(context)
{
}

void ScreenRouter::ChangeToMainScreen(ScreenManager* screenMgr)
{
    std::unique_ptr<LoadingScreen> ls = std::make_unique<LoadingScreen>();
    std::vector<std::string> fl = mainScrFileList;
    fl.insert(std::begin(fl), std::begin(blueSkyEnvMap), std::end(blueSkyEnvMap));
    ls->SetFileList(fl);
    ls->SetOnLoadedCb(
        [this, screenMgr]()
        {
            std::unique_ptr<MainScreen> scr = std::make_unique<MainScreen>();
            screenMgr->ReplaceScreen(std::move(scr), mScrContext);
        }
    );
    screenMgr->ReplaceScreen(std::move(ls), mScrContext);
}

void ScreenRouter::ChangeToGalleryScreen(ScreenManager* screenMgr)
{
    std::unique_ptr<LoadingScreen> ls = std::make_unique<LoadingScreen>();
    std::vector<std::string> fl = galleryScrFileList;
    fl.insert(std::begin(fl), std::begin(blueSkyEnvMap), std::end(blueSkyEnvMap));
    ls->SetFileList(fl);
    ls->SetOnLoadedCb(
        [this, screenMgr]()
        {
            std::unique_ptr<GalleryScreen> scr = std::make_unique<GalleryScreen>();
            screenMgr->ReplaceScreen(std::move(scr), mScrContext);
        }
    );
    screenMgr->ReplaceScreen(std::move(ls), mScrContext);
}

void ScreenRouter::ChangeToMaterialScreen(ScreenManager* screenMgr)
{
    std::unique_ptr<LoadingScreen> ls = std::make_unique<LoadingScreen>();
    std::vector<std::string> fl;
    fl.insert(std::begin(fl), std::begin(indoorsEnvMap), std::end(indoorsEnvMap));
    ls->SetFileList(fl);
    ls->SetOnLoadedCb(
        [this, screenMgr]()
        {
            std::unique_ptr<MaterialScreen> scr = std::make_unique<MaterialScreen>();
            screenMgr->ReplaceScreen(std::move(scr), mScrContext);
        }
    );
    screenMgr->ReplaceScreen(std::move(ls), mScrContext);
}

void ScreenRouter::SetupScreenRouting(ScreenManager* screenMgr)
{
    mScrContext.GetEngine()->GetWindow().AddKeyHook(
        [this, screenMgr](Key k, KeyAction ka) -> bool
        {
            if (k == Key::F1 && ka == KeyAction::Press)
                ChangeToMainScreen(screenMgr);
            else if (k == Key::F2 && ka == KeyAction::Press)
                ChangeToGalleryScreen(screenMgr);
            else if (k == Key::F3 && ka == KeyAction::Press)
                ChangeToMaterialScreen(screenMgr);
            return true;
        },
        Window::HookPos::Start
    );

    std::unique_ptr<LoadingScreen> ls = std::make_unique<LoadingScreen>();
    std::vector<std::string> fl = mainScrFileList;
    fl.insert(std::begin(fl), std::begin(blueSkyEnvMap), std::end(blueSkyEnvMap));
    ls->SetFileList(fl);
    ls->SetOnLoadedCb(
        [this, screenMgr]()
        {
            std::unique_ptr<MainScreen> mainScr = std::make_unique<MainScreen>();
            screenMgr->ReplaceScreen(std::move(mainScr), mScrContext);
        }
    );
    screenMgr->AddScreen(std::move(ls), mScrContext);
}
