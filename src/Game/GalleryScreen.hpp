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
#ifndef _GALLERY_SCREEN_HPP_
#define _GALLERY_SCREEN_HPP_

#include "Screen.hpp"
#include "../Graphics/Util/Camera.hpp"
#include "../Graphics/Scene/Scene.hpp"
#include "../Graphics/Scene/RenderformCreator.hpp"
#include "../Graphics/Renderer/Skybox.hpp"

class GalleryScreen : public Screen
{
    public:
        // Screen interface
        void onInit(ScreenContext& sc);
        void onUpdate(float dt);
        void onKey(Key k, KeyAction ka);
        void onRender(float interpolation);
        void onShutdown();
        // Transition interface
        using OnNextScreenCb = std::function<void()>;
        void SetOnNextScreenCb(OnNextScreenCb cb);
    private:
        // Engine ref
        Engine* mEngine;

        // File Data Cache ref
        ScreenContext::FileDataCache* mFileDataCache;

        // The camera view
        std::vector<Camera::MoveDirection> CameraMoveDirections();
        std::tuple<float, float> CameraLookOffset();
        Camera mCamera;

        // Scene
        std::unique_ptr<Scene> mScene;

        // Scene graphical handler converter
        std::unique_ptr<RenderformCreator> mRenderformCreator;

        // Callback called when transition event to next screen occurs
        OnNextScreenCb mOnNextScreenCb;

        // The skybox used
        std::unique_ptr<Skybox> mSkybox;

        // The irradiance skybox
        std::unique_ptr<Skybox> mIrrMap;

        // The radiance skybox
        std::unique_ptr<Skybox> mRadMap;
};

#endif // ! _GALLERY_SCREEN_HPP_
