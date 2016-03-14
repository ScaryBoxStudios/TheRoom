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
#ifndef _MAIN_SCREEN_HPP_
#define _MAIN_SCREEN_HPP_

#include "Screen.hpp"
#include "Character.hpp"
#include "../Graphics/Util/Camera.hpp"
#include "../Graphics/Scene/Scene.hpp"
#include "../Graphics/Scene/RenderformCreator.hpp"
#include "../Graphics/Renderer/Skybox.hpp"

class MainScreen : public Screen
{
    public:
        void onInit(ScreenContext& sc);
        void onUpdate(float dt);
        void onKey(Key k, KeyAction ka);
        void onRender(float interpolation);
        void onShutdown();
        // Transition interface
        using OnNextScreenCb = std::function<void()>;
        void SetOnNextScreenCb(OnNextScreenCb cb);
    private:
        // Called during initialization to setup the world
        void SetupWorld();

        // Called during initialization to setup the lights
        void SetupLights();

        //
        void UpdatePhysics(float dt);

        // Engine ref
        Engine* mEngine;

        // File Data Cache ref
        ScreenContext::FileDataCache* mFileDataCache;

        // The Scene
        void MoveCharacter() const;
        std::unique_ptr<Scene> mScene;
        bool mFollowingCharacter;
        int mMovingLightIndex;
        Character mCharacter;
        std::unique_ptr<RenderformCreator> mRenderformCreator;

        // The skybox used
        std::unique_ptr<Skybox> mSkybox;

        // The camera view
        std::vector<Camera::MoveDirection> CameraMoveDirections();
        std::tuple<float, float> CameraLookOffset();
        Camera mCamera;

        // The data needed for rotating the cubes
        struct RotationData
        {
            float degreesInc;
            bool rotating;
        };
        RotationData mRotationData;

        // When flag on AABBs are shown
        bool mShowAABBs;

        // Callback called when transition event to next screen occurs
        OnNextScreenCb mOnNextScreenCb;
};

#endif // ! _MAIN_SCREEN_HPP_
