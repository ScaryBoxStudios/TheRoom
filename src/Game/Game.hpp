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
#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <functional>
#include <vector>
#include "../Core/Engine.hpp"
#include "ScreenManager.hpp"

class Game
{
    public:
        /*! Constructor */
        Game();

        // Disable copy construction
        Game(const Game& other) = delete;
        Game& operator=(const Game& other) = delete;

        /*! Initializes all the low level modules of the game */
        void Init();

        /*! Called by the mainloop to update the game state */
        void Update(float dt);

        /*! Called by the mainloop to render the current frame */
        void Render(float interpolation);

        /*! Deinitializes all the low level modules of the game */
        void Shutdown();

        /*! Sets the master exit callback that when called should stop the main loop */
        void SetExitHandler(std::function<void()> f);

    private:
        // Called during initialization to setup window and input
        void SetupWindow();

        //
        bool mShouldChangeScreen;

        // The engine instance
        Engine mEngine;

        // The screen manager instance
        ScreenManager mScreenManager;

        // Master switch, called when game is exiting
        std::function<void()> mExitHandler;
};

#endif // ! _GAME_HPP_
