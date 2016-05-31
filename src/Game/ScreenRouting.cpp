#include "ScreenRouting.hpp"
#include "LoadingScreen.hpp"
#include "MainScreen.hpp"
#include "GalleryScreen.hpp"

ScreenRouter::ScreenRouter(ScreenContext context)
  : mScrContext(context)
{
}

void ScreenRouter::SetupScreenRouting(ScreenManager* screenMgr)
{
    mOnMainScrNext = [this, screenMgr]()
    {
        std::unique_ptr<GalleryScreen> scr = std::make_unique<GalleryScreen>();
        scr->SetOnNextScreenCb(mOnGalleryScrNext);
        screenMgr->ReplaceScreen(std::move(scr), mScrContext);
    };

    mOnGalleryScrNext = [this, screenMgr]()
    {
        std::unique_ptr<MainScreen> scr = std::make_unique<MainScreen>();
        scr->SetOnNextScreenCb(mOnMainScrNext);
        screenMgr->ReplaceScreen(std::move(scr), mScrContext);
    };

    std::unique_ptr<LoadingScreen> ls = std::make_unique<LoadingScreen>();
    ls->SetOnLoadedCb(
        [this, screenMgr]()
        {
            std::unique_ptr<MainScreen> mainScr = std::make_unique<MainScreen>();
            mainScr->SetOnNextScreenCb(mOnMainScrNext);
            screenMgr->ReplaceScreen(std::move(mainScr), mScrContext);
        }
    );
    screenMgr->AddScreen(std::move(ls), mScrContext);
}
