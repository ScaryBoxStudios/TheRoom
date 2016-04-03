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

#include <vector>
#include <memory>
#include <glad/glad.h>
#include "../Scene/Transform.hpp"
#include "../Shader/Shader.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class ShadowRenderer
{
    public:
        // Input scene data
        struct IntMesh
        {
            Transform transform;
            GLuint    vaoId,
                      eboId,
                      numIndices;
        };

        // Initializes the renderer state
        void Init(unsigned int width, unsigned int height);

        // Deinitializes the renderer state
        void Shutdown();

        // Renders the scene from the light's view in the depth buffer
        void Render(float interpolation, std::vector<IntMesh> scene);

        // Sets the light properties
        void SetLightViewParams(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& lightPos);

        // Retrieves the depth map Id
        GLuint DepthMapId() const;

        // Retrieves various shadow frustum properties
        unsigned int GetSplitNum() const;
        const std::vector<glm::mat4> GetSplitProjMats() const;
        const std::vector<glm::mat4> GetSplitViewMats() const;
        const std::vector<glm::mat4> GetSplitShadowMats() const;
        const std::vector<glm::vec2> GetSplitPlanes() const;
        const std::vector<float> GetSplitNearPlanes() const;
        const std::vector<float> GetSplitFarPlanes() const;

    private:
        // Dimensions of each shadow map
        int mWidth, mHeight;

        // GL handles
        GLuint mDepthMapId;
        GLuint mDepthMapFboId;
        std::unique_ptr<ShaderProgram> mProgram;

        // Split data
        unsigned int mSplitNum;
        std::vector<glm::mat4> mProjectionMats;
        std::vector<glm::mat4> mViewMats;
        std::vector<glm::mat4> mShadowMats;
        std::vector<glm::vec2> mPlanes;
        std::vector<float> mNearPlanes, mFarPlanes;
};

#endif // ! _SHADOW_RENDERER_HPP_
