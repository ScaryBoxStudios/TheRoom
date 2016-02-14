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
#ifndef _SCREEN_HPP_
#define _SCREEN_HPP_

#include "../Core/Engine.hpp"

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

class ScreenContext
{
    public:
        using BufferTypePtr = std::unique_ptr<BufferType>;
        using FileDataCache = std::unordered_map<std::string, BufferTypePtr>;

        ScreenContext(Engine* e, FileDataCache* fdc);
        Engine* GetEngine();

        FileDataCache* GetFileDataCache();
    private:
        Engine* mEngine;

        // File data cache
        FileDataCache* mFileDataCache;
};

class Screen
{
    public:
        // When initializing the current screen
        virtual void onInit(ScreenContext& sc) = 0;
        // Logic update
        virtual void onUpdate(float dt) = 0;
        // Key input
        virtual void onKey(Key k, KeyAction ka);
        // Mouse input
        virtual void onMouse(MouseButton mb, ButtonAction ba);
        // Render frame
        virtual void onRender(float interpoation) = 0;
        // When deinitializing the current screen
        virtual void onShutdown() = 0;
        // To be called when the current screen is exiting
        void Finish();
};

#endif // ! _SCREEN_HPP_
