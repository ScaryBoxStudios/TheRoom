#include "ScreenRouting.hpp"
#include "LoadingScreen.hpp"
#include "MainScreen.hpp"
#include "MaterialScreen.hpp"

void SetupScreenRouting(ScreenManager* screenMgr, ScreenContext& sc)
{
    // The screen context passed to screen manager actions
    ScreenContext scrCtx = sc;

    // Initialize first screen
    std::unique_ptr<LoadingScreen> ls = std::make_unique<LoadingScreen>();
    ls->SetOnLoadedCb(
        [screenMgr, &scrCtx]()
        {
            std::unique_ptr<MainScreen> mainScr = std::make_unique<MainScreen>();
            mainScr->SetOnNextScreenCb(
                [screenMgr, &scrCtx]()
                {
                    std::unique_ptr<MaterialScreen> matScr = std::make_unique<MaterialScreen>();
                    screenMgr->AddScreen(std::move(matScr), scrCtx);
                }
            );
            screenMgr->AddScreen(std::move(mainScr), scrCtx);
        }
    );

    // Push it to the engine manager
    screenMgr->AddScreen(std::move(ls), scrCtx);
}
