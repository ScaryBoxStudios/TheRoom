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
#ifndef _SCREEN_MANAGER_HPP_
#define _SCREEN_MANAGER_HPP_

#include <vector>
#include <memory>
#include <queue>
#include "Screen.hpp"

class ScreenManager
{
    public:
        using ScreenPtr = std::unique_ptr<Screen>;

        // Appends a screen the the screen stack
        void AddScreen(ScreenPtr screen, ScreenContext& sc);

        // Replaces current active screen with the given one
        void ReplaceScreen(ScreenPtr screen, ScreenContext& sc);

        // Retrieves the currently active screen
        Screen* GetActiveScreen();

        // Performs all currently queued up actions
        void PerfomQueuedActions();

    private:
        // Deferred action type
        enum class Action
        {
            Add = 0,
            Replace,
        };

        // Action data
        struct ActionData
        {
            std::unique_ptr<Screen> screen;
            ScreenContext sc;
        };

        // Actual implementations of the actions
        void AddScreenImpl(ScreenPtr screen, ScreenContext& sc);
        void ReplaceScreenImpl(ScreenPtr screen, ScreenContext& sc);

        // Deferred action queue
        std::queue<std::pair<Action, ActionData>> mDeferredActions;

        // Screen stack
        std::vector<ScreenPtr> mScreens;
};

#endif // ! _SCREEN_MANAGER_HPP_
