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
      "ext/Assets/Textures/Skybox/Bluesky/bluesky.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_irr.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_0.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_1.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_2.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_3.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_4.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_5.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_6.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_7.tga"
    , "ext/Assets/Textures/Skybox/Bluesky/bluesky_rad_8.tga"
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
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_7.tga"
    , "ext/Assets/Textures/Skybox/Indoors/indoors_rad_8.tga"
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
