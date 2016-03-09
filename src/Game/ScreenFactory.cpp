#include "ScreenFactory.hpp"
#include "LoadingScreen.hpp"
#include "MainScreen.hpp"

auto ScreenFactory::CreateScreen(ScreenName name) -> ScreenPtr
{
    ScreenPtr screen = nullptr;
    switch(name)
    {
        case ScreenName::LoadingScreen:
            screen = std::make_unique<LoadingScreen>();
            break;
        case ScreenName::MainScreen:
            screen = std::make_unique<MainScreen>();
            break;
    }
    return screen;
}
