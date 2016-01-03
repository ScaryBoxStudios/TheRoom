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
#ifndef _SKYBOX_HPP_
#define _SKYBOX_HPP_

#include <unordered_map>
#include <memory>
#include <glad/glad.h>
#include <GL/gl.h>
#include "../Image/RawImage.hpp"
#include "../Shader/Shader.hpp"
#include "../../Util/WarnGuard.hpp"
#include "../../Util/Hash.hpp"

WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class Skybox
{
    public:
        // The face of the Skybox
        enum class Target
        {
            Right  = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            Left   = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            Top    = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            Bottom = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            Back   = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            Front  = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        };

        // Constructor
        Skybox();

        // Destructor
        ~Skybox();

        // Loads the given image data to the current Skybox
        void Load(const std::unordered_map<Target, RawImage<>>& images);

        // Renders the current Skybox
        void Render(const glm::mat4& projection, const glm::mat4& view) const;

    private:
        GLuint mVao, mVbo;
        GLuint mTextureId;
        std::unique_ptr<ShaderProgram> mProgram;
};

#endif // ! _SKYBOX_HPP_
