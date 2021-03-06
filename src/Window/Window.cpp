#include "Window.hpp"
#include <sstream>
#include <iomanip>

///==============================================================
///= Window
///==============================================================
Window::Window() :
    mWindow(0),
    mTitle(""),
    mCursorX(0),
    mCursorY(0),
    mPrevCursorX(0),
    mPrevCursorY(0),
    mShowStats(false),
    mPrevTimeTicks(0)
{
    for (int i = 0; i < (int) Key::KeyLast; ++i)
        mKeyState[static_cast<Key>(i)] = KeyAction::Release;
}

Window::~Window()
{
    if (mWindow)
        Destroy();
}

bool Window::Create(int width, int height, const std::string& title, Mode mode)
{
    // Open GL version hints
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Get monitor value according to given window mode
    GLFWmonitor* mon = 0;
    switch (mode)
    {
        case Mode::Windowed:
        {
            mon = 0;
            break;
        }
        case Mode::BorderlessWindow:
        {
            mon = glfwGetPrimaryMonitor();

            const GLFWvidmode* videoMode = glfwGetVideoMode(mon);
            glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);

            break;
        }
        case Mode::Fullscreen:
        {
            mon = glfwGetPrimaryMonitor();
            break;
        }
    }

    // Create the window
    mWindow = glfwCreateWindow(width, height, title.c_str(), mon, 0);
    if(!mWindow)
        return false;

    // Store the given title
    mTitle = title;

    // Set it as the current opengl context
    glfwMakeContextCurrent(mWindow);

    // Disable vSync
    glfwSwapInterval(0);

    // Set window user pointer to this in order to be able to use our callbacks
    glfwSetWindowUserPointer(mWindow, this);

    // Load OpenGL extensions
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Window creation success
    return true;
}

void Window::Destroy()
{
    glfwDestroyWindow(mWindow);
    mWindow = 0;
}

void Window::SetTitle(const std::string& title)
{
    mTitle = title;
    glfwSetWindowTitle(mWindow, mTitle.c_str());
}

const std::string& Window::GetTitle() const
{
    return mTitle;
}

int Window::GetWidth() const
{
    int width;
    glfwGetWindowSize(mWindow, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    int height;
    glfwGetWindowSize(mWindow, nullptr, &height);
    return height;
}

void Window::SetPositionChangedHandler(PositionChangedCb cb)
{
    mPosCb = cb;
    GLFWwindowposfun f = [](GLFWwindow* w, int x, int y)
    {
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        wnd->GetPositionChangedHandler()(x, y);
    };
    glfwSetWindowPosCallback(mWindow, f);
}

Window::PositionChangedCb Window::GetPositionChangedHandler() const
{
    return mPosCb;
}

void Window::SetResizeHandler(ResizeCb cb)
{
    mResizeCb = cb;
    GLFWwindowsizefun f = [](GLFWwindow* w, int width, int height)
    {
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        wnd->GetResizeHandler()(width, height);
    };
    glfwSetWindowSizeCallback(mWindow, f);
}

Window::ResizeCb Window::GetResizeHandler() const
{
    return mResizeCb;
}

void Window::SetFramebufferResizeHandler(ResizeCb cb)
{
    mFramebufferResizeCb = cb;
    GLFWwindowsizefun f = [](GLFWwindow* w, int width, int height)
    {
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        wnd->GetFramebufferResizeHandler()(width, height);
    };
    glfwSetFramebufferSizeCallback(mWindow, f);
}

Window::ResizeCb Window::GetFramebufferResizeHandler() const
{
    return mFramebufferResizeCb;
}

void Window::SetCloseHandler(CloseCb cb)
{
    mCloseCb = cb;
    GLFWwindowclosefun f = [](GLFWwindow* w)
    {
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        wnd->GetCloseHandler()();
    };
    glfwSetWindowCloseCallback(mWindow, f);
}

Window::CloseCb Window::GetCloseHandler() const
{
    return mCloseCb;
}

void Window::SetFocusChangedHandler(FocusChangedCb cb)
{
    mFocusChCb = cb;
    GLFWwindowfocusfun f = [](GLFWwindow* w, int gainedFocus)
    {
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        wnd->GetFocusChangedHandler()(gainedFocus == GL_TRUE);
    };
    glfwSetWindowFocusCallback(mWindow, f);
}

Window::FocusChangedCb Window::GetFocusChangedHandler() const
{
    return mFocusChCb;
}

void Window::SetMouseButtonPressHandler(MouseButtonPressedCb cb)
{
    mMouseBtnPressCb = cb;
    GLFWmousebuttonfun f = [](GLFWwindow* w, int button, int action, int mods)
    {
        (void) mods;
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        MouseButton mbut = static_cast<MouseButton>(button);
        ButtonAction mact = static_cast<ButtonAction>(action);
        wnd->GetMouseButtonPressHandler()(mbut, mact);
    };
    glfwSetMouseButtonCallback(mWindow, f);
}

Window::MouseButtonPressedCb Window::GetMouseButtonPressHandler() const
{
    return mMouseBtnPressCb;
}

void Window::SetCursorPositionChangedHandler(CursorPositionChangedCb cb)
{
    mCursorPosCb = cb;
    GLFWcursorposfun f = [](GLFWwindow* w, double xpos, double ypos)
    {
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        wnd->GetCursorPositionChangedHandler()(xpos, ypos);
    };
    glfwSetCursorPosCallback(mWindow, f);
}

Window::CursorPositionChangedCb Window::GetCursorPositionChangedHandler() const
{
    return mCursorPosCb;
}

void Window::SetMouseScrollHandler(MouseScrollCb cb)
{
    mMouseScrollCb = cb;
    GLFWscrollfun f = [](GLFWwindow* w, double xoffset, double yoffset)
    {
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        wnd->GetMouseScrollHandler()(xoffset, yoffset);
    };
    glfwSetScrollCallback(mWindow, f);
}

Window::MouseScrollCb Window::GetMouseScrollHandler() const
{
    return mMouseScrollCb;
}

void Window::SetKeyPressedHandler(KeyPressedCb cb)
{
    mKeyPressCb = cb;
    GLFWkeyfun f = [](GLFWwindow* w, int key, int scancode, int action, int mods)
    {
        (void) scancode;
        (void) mods;
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        Key k = static_cast<Key>(key);
        KeyAction ka = static_cast<KeyAction>(action);
        // Run through keyboard hooks first
        for (const auto& hook : wnd->mKeyHooks)
        {
            bool r = hook(k, ka);
            if (!r)
                return;
        }
        // Set key state
        wnd->mKeyState[k] = ka;

        // Call actual callback
        wnd->GetKeyPressedHandler()(k, ka);
    };
    glfwSetKeyCallback(mWindow, f);
}

Window::KeyPressedCb Window::GetKeyPressedHandler() const
{
    return mKeyPressCb;
}

void Window::SetCharEnterHandler(CharEnterCb cb)
{
    mCharEnterCb = cb;
    GLFWcharfun f = [](GLFWwindow* w, unsigned int c)
    {
        // Char as utf8
        char u8 = (char) c;
        Window* wnd = static_cast<Window*>(glfwGetWindowUserPointer(w));
        // Run through char hooks first
        for (const auto& hook : wnd->mCharHooks)
        {
            bool r = hook(u8);
            if (!r)
                return;
        }
        // Call actual callback
        wnd->GetCharEnterHandler()(u8);
    };
    glfwSetCharCallback(mWindow, f);
}

Window::CharEnterCb Window::GetCharEnterHandler() const
{
    return mCharEnterCb;
}

bool Window::IsKeyPressed(Key k) const
{
    KeyAction ka = mKeyState.at(k);
    return (ka == KeyAction::Press || ka == KeyAction::Repeat);
}

void Window::SetMouseGrabEnabled(bool on)
{
    if (on)
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool Window::MouseGrabEnabled() const
{
    return glfwGetInputMode(mWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

std::tuple<double, double> Window::GetCursorDiff() const
{
    return std::make_tuple<double, double>(mCursorX - mPrevCursorX, mCursorY - mPrevCursorY);
}

void Window::UpdateTitleStats()
{
    double elapsedMs = (glfwGetTime() * 1000) - mPrevTimeTicks;
    int FPS = static_cast<int>(elapsedMs != 0 ? 1000 / elapsedMs : -1);

    std::stringstream ss;
    ss << mTitle << " | " << "FPS: " << std::setw(4) << FPS << " Ms: " << elapsedMs;

    glfwSetWindowTitle(mWindow, ss.str().c_str());
    mPrevTimeTicks = glfwGetTime() * 1000;
}

void Window::SetShowStats(bool show)
{
    mShowStats = show;
    // Reset title in case show == false
    if (!show)
        glfwSetWindowTitle(mWindow, mTitle.c_str());
}

void Window::SwapBuffers()
{
    if(mShowStats)
        UpdateTitleStats();

    glfwSwapBuffers(mWindow);
}

void Window::Update()
{
    // Poll events from the event queue and call their callback handlers
    glfwPollEvents();

    // Update the cursor position state
    mPrevCursorX = mCursorX;
    mPrevCursorY = mCursorY;
    glfwGetCursorPos(mWindow, &mCursorX, &mCursorY);
}

void Window::AddKeyHook(KeyPressedHookCb cb, HookPos pos)
{
    switch (pos)
    {
        case HookPos::Start:
            mKeyHooks.insert(std::begin(mKeyHooks), cb);
            break;
        case HookPos::End:
            mKeyHooks.insert(std::end(mKeyHooks), cb);
            break;
    }
}

void Window::AddCharHook(CharEnterHookCb cb, HookPos pos)
{
    switch (pos)
    {
        case HookPos::Start:
            mCharHooks.insert(std::begin(mCharHooks), cb);
            break;
        case HookPos::End:
            mCharHooks.insert(std::end(mCharHooks), cb);
            break;
    }
}
