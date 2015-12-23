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
#include "../Model/ModelStore.hpp"
#include "../Shader/ShaderStore.hpp"
#include "../Texture/TextureStore.hpp"
#include "../Scene/Transform.hpp"
#include "GBuffer.hpp"
#include "../../Util/WarnGuard.hpp"

WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class Renderer
{
    public:
        /*! Initializes the renderer */
        void Init();

        /*! Called when updating the game state */
        void Update(float dt);

        /*! Called when rendering the current frame */
        void Render(float interpolation);

        /*! Deinitializes the renderer */
        void Shutdown();

        /*! Sets the view matrix */
        void SetView(const glm::mat4& view);

        /*! Retrieves the renderer's TextureStore */
        TextureStore& GetTextureStore();

        /*! Retrieves the renderer's ShaderStore */
        ShaderStore& GetShaderStore();

        /*! Retrieves the renderer's ModelStore */
        ModelStore& GetModelStore();

        // WorldObject
        struct WorldObject
        {
            Transform transform;
            std::string model;
        };

        /*! Retrieves the renderer's World */
        std::unordered_map<std::string, std::vector<WorldObject>>& GetWorld();

        // Store light separately
        WorldObject* mLight;

    private:
        // Performs the geometry pass rendering step
        void GeometryPass(float interpolation);

        // Performs the light pass rendering step
        void LightPass(float interpolation);

        // Renders a 1x1 quad in NDC, used for framebuffer color targets
        void RenderQuad();

        //
        //void ForwardRender(float interpolation);

        // The view matrix
        glm::mat4 mView;

        // Stores the world objects
        std::unordered_map<std::string, std::vector<WorldObject>> mWorld;

        // Stores the models loaded in the gpu
        ModelStore mModelStore;

        // Stores the shaders and shader programs loaded in the gpu
        ShaderStore mShaderStore;

        // Stores the textures loaded in the gpu
        TextureStore mTextureStore;

        // The GBuffer used by the deffered rendering steps
        std::unique_ptr<GBuffer> mGBuffer;
};

#endif // ! _RENDERER_HPP_
