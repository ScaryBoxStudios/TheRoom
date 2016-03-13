#include "ScreenRouting.hpp"
#include "LoadingScreen.hpp"
#include "MainScreen.hpp"
#include "MaterialScreen.hpp"

ScreenRouter::ScreenRouter(ScreenContext context)
  : mScrContext(context)
{
}

void ScreenRouter::SetupScreenRouting(ScreenManager* screenMgr)
{
    mOnMainScrNext = [this, screenMgr]()
    {
        std::unique_ptr<MaterialScreen> matScr = std::make_unique<MaterialScreen>();
        matScr->SetOnNextScreenCb(mOnMatScrNext);
        screenMgr->ReplaceScreen(std::move(matScr), mScrContext);
    };

    mOnMatScrNext = [this, screenMgr]()
    {
        std::unique_ptr<MainScreen> mainScr = std::make_unique<MainScreen>();
        mainScr->SetOnNextScreenCb(mOnMainScrNext);
        screenMgr->ReplaceScreen(std::move(mainScr), mScrContext);
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
