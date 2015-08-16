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
#include <functional>
#include <chrono>
#include "System/HeartBeat.hpp"
#include "Window/GlfwContext.hpp"
#include "Window/Window.hpp"

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    {
        // Initialize glfw library context
        GlfwContext glfwContext;
        glfwContext.Init();

        // Create the main loop algorithm strategy
        HeartBeat hb;
        auto exitHandler = std::bind(&HeartBeat::SetRunning, &hb, false);

        // Create the window and set its properties
        Window window;
        window.Create(800, 600, "TheRoom", Window::Mode::Windowed);
        window.SetShowFPS(true);
        window.SetCloseHandler(exitHandler);
        window.SetKeyPressedHandler(
            [&exitHandler](Key k, KeyAction ka)
            {
                if(k == Key::Escape && ka == KeyAction::Release)
                    exitHandler();
            }
        );

        // Set the time source
        hb.SetTimer([]
            {
                return static_cast<double>(
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count()
                );
            }
        );

        // Set the state update callback
        hb.SetUpdate([&window](float dt)
            {
                (void) dt;
                window.PollEvents();
            }
        );

        // Set the render callback
        hb.SetRender([&window](float interpolation)
            {
                (void) interpolation;
                window.SwapBuffers();
            }
        );
        
        // Run the main loop
        hb.Run();

        // Destroy the created window
        window.Destroy();

        // Uninitialize the glfw library context
        glfwContext.Shutdown();
    }

    return 0;
}

