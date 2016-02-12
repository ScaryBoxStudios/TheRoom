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
#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include <memory>
#include "GBuffer.hpp"
#include "Light.hpp"
#include "Skybox.hpp"
#include "TextRenderer.hpp"
#include "AABBRenderer.hpp"
#include "ShadowRenderer.hpp"
#include "../Geometry/ModelStore.hpp"
#include "../Scene/Scene.hpp"
#include "../Scene/Transform.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class Renderer
{
    public:
        /*! Initializes the renderer */
        void Init(int width, int height, GLuint gPassProgId, GLuint lPassProgId);

        /*! Called when updating the game state */
        void Update(float dt);

        /*! Called when rendering the current frame */
        void Render(float interpolation);

        /*! Deinitializes the renderer */
        void Shutdown();

        /*! Sets the view matrix */
        void SetView(const glm::mat4& view);

        /*! Sets the skybox */
        void SetSkybox(const Skybox* s);

        /*! Sets the current rendering scene */
        void SetScene(const Scene* scene);

        /*! Retrieves the renderer's ModelStore */
        ModelStore& GetModelStore();

        /*! Toggles showing the AABBs */
        void ToggleShowAABBs();

    private:
        // Performs the geometry pass rendering step
        void GeometryPass(float interpolation);

        // Performs the light pass rendering step
        void LightPass(float interpolation);

        // The projection matrix
        glm::mat4 mProjection;

        // The view matrix
        glm::mat4 mView;

        // The screen size
        int mScreenWidth, mScreenHeight;

        // Currently rendering scene
        const Scene* mScene;

        // The lights that are rendered
        Lights mLights;

        // Stores the models loaded in the gpu
        ModelStore mModelStore;

        // Shader programIds of the geometry pass and the lighting pass
        GLuint mGeometryPassProgId, mLightingPassProgId;

        // The GBuffer used by the deffered rendering steps
        std::unique_ptr<GBuffer> mGBuffer;

        // The root Skybox used
        const Skybox* mSkybox;

        // The AABB rendering utility
        AABBRenderer mAABBRenderer;

        // The shadow map rendering utility
        ShadowRenderer mShadowRenderer;

        // When flag on AABBs are shown
        bool mShowAABBs;
};

#endif // ! _RENDERER_HPP_
