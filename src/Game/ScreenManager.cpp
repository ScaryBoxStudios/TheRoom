#include "ScreenManager.hpp"

void ScreenManager::AddScreen(std::unique_ptr<Screen> screen, ScreenContext& sc)
{
    mScreens.push_back(std::move(screen));
    mScreens.back()->onInit(sc);
}

void ScreenManager::ReplaceScreen(std::unique_ptr<Screen> screen, ScreenContext& sc)
{
    //mScreens.back()->onPause(sc);
    mScreens.push_back(std::move(screen));
    mScreens.back()->onInit(sc);
}

Screen* ScreenManager::GetActiveScreen()
{
    if (mScreens.empty())
        return nullptr;
    else
        return mScreens.back().get();
}
