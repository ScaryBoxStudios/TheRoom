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
#ifndef _LOADING_SCREEN_HPP_
#define _LOADING_SCREEN_HPP_

#include "Screen.hpp"
#include <functional>
#include <unordered_map>

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

class LoadingScreen : public Screen
{
    public:
        // Screen interface
        void onInit(ScreenContext& sc);
        void onUpdate(float dt);
        void onRender(float interpolation);
        void onShutdown();

        // Finish callback
        using FinishCb = std::function<void()>;
        void SetFinishCb(FinishCb cb);
    private:
        // Loads file data into memory cache
        void LoadFileData();
        // Master load function called when file data have been loaded in cache
        void LoadFromMem();
            // Called during initialization to load textures
            void LoadTextures();
            // Called during initialization to load models
            void LoadModels();

        // Engine ref
        Engine* mEngine;
        // File data cache
        using BufferTypePtr = std::unique_ptr<BufferType>;
        std::unordered_map<std::string, BufferTypePtr> mFileDataCache;
        // Indicates that data files have been loaded to cache
        bool mFileCacheIsReady;
        // Holds the currently loading file
        std::string mCurrentlyLoading;
        // Finish callback
        FinishCb mFinishCb;
};

#endif // ! _LOADING_SCREEN_HPP_
