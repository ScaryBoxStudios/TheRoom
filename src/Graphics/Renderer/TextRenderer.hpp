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
#ifndef _TEXT_RENDERER_HPP_
#define _TEXT_RENDERER_HPP_

#include <string>
#include <memory>
#include <glad/glad.h>
#include "../Resource/FontStore.hpp"
#include "../Shader/Shader.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class TextRenderer
{
    public:
        /*! Initializes the renderer */
        void Init(int width, int height);

        /*! Called when the viewport dimensions change */
        void Resize(int width, int height);

        /*! Called when rendering the current frame */
        void RenderText(const std::string& text, float x, float y, int pixelHeight, glm::vec3 color, const std::string& font);

        /*! Deinitializes the renderer */
        void Shutdown();

        /*! Retrieves the internal FontStore instance */
        FontStore& GetFontStore();

    private:
        // The fontstore instance
        FontStore mFontStore;

        // The orho matrix used when rendering
        glm::mat4 mProjection;

        // The rendering quad info
        GLuint mVao, mVbo;

        // The font renderer OpenGL program
        std::unique_ptr<ShaderProgram> mProgram;
};

#endif // ! _TEXT_RENDERER_HPP_
