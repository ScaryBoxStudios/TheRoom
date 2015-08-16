#include "Window.hpp"
#include <sstream>
#include <iomanip>

///==============================================================
///= Window
///==============================================================
Window::Window() :
    mWindow(0),
    mTitle(""),
    mShowFPS(false),
    mPrevTimeTicks(0)
{
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

            const GLFWvidmode* mode = glfwGetVideoMode(mon);
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

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
        wnd->GetKeyPressedHandler()(k, ka);
    };
    glfwSetKeyCallback(mWindow, f);
}

Window::KeyPressedCb Window::GetKeyPressedHandler() const
{
    return mKeyPressCb;
}

void Window::UpdateTitleFPS()
{
    double elapsedMs = (glfwGetTime() * 1000) - mPrevTimeTicks;
    int FPS = static_cast<int>(elapsedMs != 0 ? 1000 / elapsedMs : -1);

    std::stringstream ss;
    ss << mTitle << " | " << "FPS: " << std::setw(4) << FPS;

    glfwSetWindowTitle(mWindow, ss.str().c_str());
    mPrevTimeTicks = glfwGetTime() * 1000;
}

void Window::SetShowFPS(bool show)
{
    mShowFPS = show;
    // Reset title in case show == false
    if (!show)
        glfwSetWindowTitle(mWindow, mTitle.c_str());
}

void Window::SwapBuffers()
{
    if(mShowFPS)
        UpdateTitleFPS();

    glfwSwapBuffers(mWindow);
}

void Window::PollEvents()
{
    glfwPollEvents();
}

