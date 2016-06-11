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
#ifndef _CUBEMAP_STORE_HPP_
#define _CUBEMAP_STORE_HPP_

#include <unordered_map>
#include <string>
#include <glad/glad.h>
#include "../../Asset/Image/RawImage.hpp"
#include "../../Util/Hash.hpp"

struct CubemapDescription
{
    GLuint id;
};

class CubemapStore
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
        CubemapStore();

        // Destructor
        ~CubemapStore();

        // Disable copy construction
        CubemapStore(const CubemapStore&) = delete;
        CubemapStore& operator=(const CubemapStore&) = delete;

        // Enable move construction
        CubemapStore(CubemapStore&&) = default;
        CubemapStore& operator=(CubemapStore&&) = default;

        // Loads the given image data to the current Cubemap
        void Load(const std::string& name, const std::unordered_map<Target, RawImage>& images, GLuint level = 0);

        // Loads image data from cross formatted image
        void Load(const std::string& name, const RawImage& cross, GLuint level = 0);

        // Retrieves a pointer to a loaded cubemap object
        CubemapDescription* operator[](const std::string& name);

        // Unloads stored textures in the store
        void Clear();

    private:
        std::unordered_map<std::string, CubemapDescription> mCubemaps;
};

#endif // ! _CUBEMAP_STORE_HPP_
