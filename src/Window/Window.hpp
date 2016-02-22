/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <string>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Input.hpp"

///==============================================================
/// Window
///  * Manages window creation and actions and event dispatching
///==============================================================
class Window
{
    public:
        /// The modes in witch a window can be created
        enum class Mode
        {
            Windowed,
            BorderlessWindow,
            Fullscreen
        };

        /// Window event callback types
        using PositionChangedCb = std::function<void(int, int)>;
        using ResizeCb = std::function<void(int, int)>;
        using CloseCb = std::function<void()>;
        using FocusChangedCb = std::function<void(bool)>;

        /// Input event callback types
        using MouseButtonPressedCb = std::function<void(MouseButton, ButtonAction)>;
        using CursorPositionChangedCb = std::function<void(double, double)>;
        using MouseScrollCb = std::function<void(double, double)>;
        using KeyPressedCb = std::function<void(Key, KeyAction)>;

        /// Default ctor
        Window();

        /// Dctor
        ~Window();

        /// Create the window with the given properties
        bool Create(int width, int height, const std::string& title, Mode mode);

        /// Destroy the window on demand
        void Destroy();

        /// Changes window title to given one
        void SetTitle(const std::string& title);

        /// Retrieves the window title
        const std::string& GetTitle() const;

        /// Retrieves the window width (client area) in pixels
        int GetWidth() const;

        /// Retrieves the window height (client area) in pixels
        int GetHeight() const;

        /// Sets the position changed callback handler
        void SetPositionChangedHandler(PositionChangedCb cb);

        /// Retrieves the position changed callback handler
        PositionChangedCb GetPositionChangedHandler() const;

        /// Sets the window resize callback handler
        void SetResizeHandler(ResizeCb cb);

        /// Retrieves the window resize callback handler
        ResizeCb GetResizeHandler() const;

        /// Sets the window close callback handler
        void SetCloseHandler(CloseCb cb);

        /// Retrieves the window close callback handler
        CloseCb GetCloseHandler() const;

        /// Sets the window focus changed callback handler
        void SetFocusChangedHandler(FocusChangedCb cb);

        /// Retrieves the window focus changed callback handler
        FocusChangedCb GetFocusChangedHandler() const;

        /// Sets the mouse button press callback handler
        void SetMouseButtonPressHandler(MouseButtonPressedCb cb);

        /// Retrieves the mouse button press callback handler
        MouseButtonPressedCb GetMouseButtonPressHandler() const;

        /// Sets the cursor position changed callback handler
        void SetCursorPositionChangedHandler(CursorPositionChangedCb cb);

        /// Retrieves the cursor position changed callback handler
        CursorPositionChangedCb GetCursorPositionChangedHandler() const;

        /// Sets the mouse scroll event callback handler
        void SetMouseScrollHandler(MouseScrollCb cb);

        /// Retrieves the mouse scroll event callback handler
        MouseScrollCb GetMouseScrollHandler() const;

        /// Sets the key press callback handler
        void SetKeyPressedHandler(KeyPressedCb cb);

        /// Retrieves the key press callback handler
        KeyPressedCb GetKeyPressedHandler() const;

        /// Returns true if in the frame that the function was called, the given Key was pressed
        bool IsKeyPressed(Key k) const;

        /// Enables or Disables mouse grab
        void SetMouseGrabEnabled(bool on);

        /// Retrieves the value of the mouse grab option
        bool MouseGrabEnabled() const;

        /// Retrieves the relative movement difference of the cursor from the last frame
        std::tuple<double, double> GetCursorDiff() const;

        /// Enables debug option that appends current fps and frame ms to the window title
        void SetShowStats(bool show);

        /// Swaps the back with the front frame buffer
        void SwapBuffers();

        /// Peeks for the events of the current window and feeds them to the registered callbacks
        /// Updates also the cursor state variables for making the Cursor Diff functions usable
        void Update();

    private:
        // Main window properties
        GLFWwindow* mWindow;
        std::string mTitle;

        // The stored callback handlers
        PositionChangedCb mPosCb;
        ResizeCb mResizeCb;
        CloseCb mCloseCb;
        FocusChangedCb mFocusChCb;
        MouseButtonPressedCb mMouseBtnPressCb;
        CursorPositionChangedCb mCursorPosCb;
        MouseScrollCb mMouseScrollCb;
        KeyPressedCb mKeyPressCb;

        // Appends current statistics to the window title if flag is true
        void UpdateTitleStats();

        // Stores the cursor position from the last two frames
        double mCursorX, mCursorY, mPrevCursorX, mPrevCursorY;

        // State variables used by the window title stats debugging feature
        bool mShowStats;
        double mPrevTimeTicks;
};

#endif // ! _WINDOW_HPP_

