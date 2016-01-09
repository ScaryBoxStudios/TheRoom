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
#ifndef _SHADOW_RENDERER_HPP_
#define _SHADOW_RENDERER_HPP_

#include <memory>
#include <glad/glad.h>
#include <GL/gl.h>
#include "../Shader/Shader.hpp"
#include "../Scene/Scene.hpp"
#include "../Model/ModelStore.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class ShadowRenderer
{
    public:
        // Initializes the renderer state
        void Init(unsigned int width, unsigned int height);

        // Deinitializes the renderer state
        void Shutdown();

        // Renders the scene from the light's view in the depth buffer
        void Render(float interpolation);

        // Sets the scene to process
        void SetScene(const Scene* scene);

        // Sets the model store to retrieve the Model data for the scene
        void SetModelStore(ModelStore* modelStore);

        // Sets the light properties
        void SetLightPos(const glm::vec3& lightPos);

        // Retrieves the depth map Id
        GLuint DepthMapId() const;

    private:
        int mWidth, mHeight;

        GLuint mDepthMapId;
        GLuint mDepthMapFboId;
        std::unique_ptr<ShaderProgram> mProgram;

        glm::vec3 mLightPos;
        const Scene* mScene;
        ModelStore* mModelStore;
};

#endif // ! _SHADOW_RENDERER_HPP_
