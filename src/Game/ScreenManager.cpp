#include "ScreenManager.hpp"

void ScreenManager::AddScreen(ScreenPtr screen, ScreenContext& sc)
{
    ActionData actionData =
    {
        std::move(screen),
        sc
    };
    mDeferredActions.push(std::make_pair(Action::Add, std::move(actionData)));
}

void ScreenManager::ReplaceScreen(ScreenPtr screen, ScreenContext& sc)
{
    ActionData actionData =
    {
        std::move(screen),
        sc
    };
    mDeferredActions.push(std::make_pair(Action::Replace, std::move(actionData)));
}

Screen* ScreenManager::GetActiveScreen()
{
    if (mScreens.empty())
        return nullptr;
    else
        return mScreens.back().get();
}

void ScreenManager::PerfomQueuedActions()
{
    while (mDeferredActions.size() != 0)
    {
        // Pop action
        std::pair<Action, ActionData> actionPair = std::move(mDeferredActions.front());
        mDeferredActions.pop();

        // Extract the data from the ActionData structure
        ScreenPtr screen = std::move(actionPair.second.screen);
        ScreenContext sc = std::move(actionPair.second.sc);

        // Call action type handler
        switch (actionPair.first)
        {
            case Action::Add:
            {
                AddScreenImpl(std::move(screen), sc);
                break;
            }
            case Action::Replace:
            {
                ReplaceScreenImpl(std::move(screen), sc);
                break;
            }
        }
    }
}

void ScreenManager::AddScreenImpl(std::unique_ptr<Screen> screen, ScreenContext& sc)
{
    mScreens.push_back(std::move(screen));
    mScreens.back()->onInit(sc);
}

void ScreenManager::ReplaceScreenImpl(std::unique_ptr<Screen> screen, ScreenContext& sc)
{
    //mScreens.back()->onPause(sc);
    mScreens.push_back(std::move(screen));
    mScreens.back()->onInit(sc);
}
