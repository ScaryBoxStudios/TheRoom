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
#include <vector>
#include "GBuffer.hpp"
#include "Light.hpp"
#include "ShadowRenderer.hpp"
#include "../Scene/Transform.hpp"
#include "../Material/MaterialStore.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class Renderer
{
    public:
        struct IntMesh
        {
            Transform transformation;
            GLuint    vaoId,
                      eboId,
                      numIndices;
        };

        struct IntMaterial
        {
            GLuint diffTexId,
                   specTexId,
                   nmapTexId,
                   matIndex;
            bool   useNormalMap;
        };

        using MaterialVecEntry = std::pair<IntMaterial, std::vector<IntMesh>>;

        struct IntForm
        {
            std::vector<MaterialVecEntry> materials;
            GLuint                        skyboxId    = 0;
            GLuint                        skysphereId = 0;
        };

        /*! Initializes the renderer */
        void Init(int width, int height, GLuint gPassProgId, GLuint lPassProgId);

        /*! Called when window is resized */
        void Resize(int width, int height);

        /*! Called when updating the game state */
        void Update(float dt);

        /*! Called when rendering the current frame */
        void Render(float interpolation, const IntForm& intForm);

        /*! Deinitializes the renderer */
        void Shutdown();

        /*! Sets the various data stores that hold the GPU handles to data */
        void SetDataStores(MaterialStore* matStore);

        /*! Sets the view matrix */
        void SetView(const glm::mat4& view);

        /*! Retrieves the renderer's Lights */
        Lights& GetLights();

        /*! Retrieves the renderer's cached projection matrix */
        const glm::mat4 GetProjection() const;

    private:
        // Performs the geometry pass rendering step
        void GeometryPass(float interpolation, const IntForm& intForm);

        // Performs the light pass rendering step
        void LightPass(float interpolation, const IntForm& intForm);

        // Performs a stencil pass
        void StencilPass(const PointLight& pLight);

        // The projection matrix
        glm::mat4 mProjection;

        // The view matrix
        glm::mat4 mView;

        // The screen size
        int mScreenWidth, mScreenHeight;

        // The lights that are rendered
        Lights mLights;

        // References to various data stores
        MaterialStore* mMaterialStore;

        // Shader programIds of the geometry pass and the lighting pass
        GLuint mGeometryPassProgId, mLightingPassProgId;

        // The GBuffer used by the deffered rendering steps
        std::unique_ptr<GBuffer> mGBuffer;

        // The null program used by the stencil passes
        std::unique_ptr<ShaderProgram> mNullProgram;

        // The shadow map rendering utility
        ShadowRenderer mShadowRenderer;

        // Uniform Buffer objects
        GLuint mUboMatrices;
};

#endif // ! _RENDERER_HPP_
