#include "Game.hpp"
#include <algorithm>
#include "LoadingScreen.hpp"
#include "MainScreen.hpp"

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

    //
    mShouldChangeScreen = false;

    // Initialize first screen
    ScreenContext sc(&mEngine, &mFileDataCache);
    std::unique_ptr<LoadingScreen> ls = std::make_unique<LoadingScreen>();
    ls->SetFinishCb([this]() { mShouldChangeScreen = true; });
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

    // Load main screen
    if (mShouldChangeScreen)
    {
        ScreenContext sc(&mEngine, &mFileDataCache);
        mScreenManager.AddScreen(std::make_unique<MainScreen>(), sc);
        mShouldChangeScreen = false;
    }

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
