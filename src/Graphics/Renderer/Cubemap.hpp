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
#ifndef _CUBEMAP_HPP_
#define _CUBEMAP_HPP_

#include <unordered_map>
#include <glad/glad.h>
#include "../../Asset/Image/RawImage.hpp"
#include "../../Util/Hash.hpp"

class Cubemap
{
    public:
        // The face of the Cubemap
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
        Cubemap();

        // Destructor
        ~Cubemap();

        // Retrieves the cubemap handle
        GLuint Id() const;

        // Loads the given image data to the current Cubemap
        void SetData(const std::unordered_map<Target, RawImage<>>& images, GLuint level = 0);

    private:
        GLuint mTextureId;
};

#endif // ! _CUBEMAP_HPP_
