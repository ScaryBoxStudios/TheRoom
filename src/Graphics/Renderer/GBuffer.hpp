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
#ifndef _G_BUFFER_HPP_
#define _G_BUFFER_HPP_

#include <glad/glad.h>
#include <GL/gl.h>

class GBuffer
{
    public:
        // Constructor
        GBuffer(unsigned int width, unsigned int height);

        // Destructor
        ~GBuffer();

        enum class Mode
        {
            GeometryPass,
            LightPass,
            StencilPass
        };
        // Sets the GBuffer mode by binding the relevant internal data
        void PrepareFor(Mode mode);

        // Retrieves the internal GBuffer id
        GLuint Id() const;

        // Retrieves the position buffer id
        GLuint PosId() const;

        // Retrieves the normal buffer id
        GLuint NormalId() const;

        // Retrieves the albedo-specular buffer id
        GLuint AlbedoSpecId() const;

    private:
        // The GBuffer's framebuffer id
        GLuint mGBufferId;
        // The position, normal and albedo + specular buffer ids'
        GLuint mPositionBufId, mNormalBufId, mAlbedoSpecBufId;
        // The depth buffer id
        GLuint mDepthBufId;
};

#endif // ! _G_BUFFER_HPP_
