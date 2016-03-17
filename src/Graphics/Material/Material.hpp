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
#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include <glad/glad.h>
#include <GL/gl.h>

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class Material
{
    public:
        // Constructor, default material
        Material();

        // Diffuse color
        const glm::vec3& GetDiffuseColor() const;
        void SetDiffuseColor(const glm::vec3& v);
        // Diffuse texture
        GLuint GetDiffuseTexture() const;
        void SetDiffuseTexture(GLuint id);
        bool UsesDiffuseTexture() const;

        // Specular color
        const glm::vec3& GetSpecularColor() const;
        void SetSpecularColor(const glm::vec3& v);
        // Specular texture
        GLuint GetSpecularTexture() const;
        void SetSpecularTexture(GLuint id);
        bool UsesSpecularTexture() const;

        // Emissive color
        const glm::vec3& GetEmissiveColor() const;
        void SetEmissiveColor(const glm::vec3& v);

        // Shininesss
        float GetRoughness() const;
        void SetRoughness(float s);

        // Fresnel
        float GetFresnel() const;
        void SetFresnel(float f);

        // Normal Map texture
        GLuint GetNormalMapTexture() const;
        void SetNormalMapTexture(GLuint id);
        bool UsesNormalMapTexture() const;

    private:
        // Diffuse properties
        glm::vec3 mDiffColor;
        GLuint mDiffTexture;

        // Specular properties
        glm::vec3 mSpecColor;
        GLuint mSpecTexture;

        // Emissive properties
        glm::vec3 mEmissiveColor;

        // Roughness
        float mRoughness;

        // Fresnel
        float mFresnel;

        // Normal mapping properties
        GLuint mNMapTexture;
};

#endif // ! _MATERIAL_HPP_
