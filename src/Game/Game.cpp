#include "Game.hpp"
#include <algorithm>
#include "ScreenFactory.hpp"

///==============================================================
///= Game
///==============================================================
Game::Game()
{
}

void Game::Init()
{
    // Initialize the engine instance
    mEngine.Init();

    // Setup window and input
    SetupWindow();

    // Initialize first screen
    ScreenFactory screenFactory;
    std::unique_ptr<Screen> ls = screenFactory.CreateScreen(ScreenFactory::ScreenName::LoadingScreen);
    ls->SetFinishCb(
        [&screenFactory, this]()
        {
            std::unique_ptr<Screen> mainScr = screenFactory.CreateScreen(ScreenFactory::ScreenName::MainScreen);
            ScreenContext sc(&mEngine, &mFileDataCache);
            mScreenManager.AddScreen(std::move(mainScr), sc);
        }
    );

    // Push it to the engine manager
    ScreenContext sc(&mEngine, &mFileDataCache);
    mScreenManager.AddScreen(std::move(ls), sc);
}

void Game::SetupWindow()
{
    auto& window = mEngine.GetWindow();
    auto& renderer = mEngine.GetRenderer();

    window.SetCloseHandler(mExitHandler);

    // Setup input event handlers
    window.SetMouseButtonPressHandler(
        [this, &window](MouseButton mb, ButtonAction ba)
        {
            if (mb == MouseButton::Left && ba == ButtonAction::Press)
                window.SetMouseGrabEnabled(true);
            mScreenManager.GetActiveScreen()->onMouse(mb, ba);
        }
    );
    window.SetKeyPressedHandler(
        [this, &window, &renderer](Key k, KeyAction ka)
        {
            // Exit
            if(k == Key::Escape && ka == KeyAction::Release)
                mExitHandler();
            // Ungrab mouse
            if(k == Key::RightControl && ka == KeyAction::Release)
                window.SetMouseGrabEnabled(false);
            // Fw to Active screen onKey
            mScreenManager.GetActiveScreen()->onKey(k, ka);
        }
    );
}

void Game::Update(float dt)
{
    // Perform deferred screen state actions
    mScreenManager.PerfomQueuedActions();

    // Check if last screen was poped out
    if (mScreenManager.GetActiveScreen() == nullptr)
        mExitHandler();
    else
        mScreenManager.GetActiveScreen()->onUpdate(dt);
}

void Game::Render(float interpolation)
{
    // Render current screen
    Screen* s = mScreenManager.GetActiveScreen();
    if (s)
        s->onRender(interpolation);

    // Pass event to engine
    mEngine.Render(interpolation);
}

void Game::Shutdown()
{
    // Shutdown the engine
    mEngine.Shutdown();
}

void Game::SetExitHandler(std::function<void()> f)
{
    mExitHandler = f;
}
