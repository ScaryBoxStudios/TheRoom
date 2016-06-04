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
#ifndef _FONT_HPP_
#define _FONT_HPP_

#include <map>
#include <vector>
#include <glad/glad.h>

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class Font
{
    public:
        // Glyph
        struct Glyph
        {
            GLuint texId;
            glm::ivec2 size;
            glm::ivec2 bearing;
            long advance;
        };

        // Constructor
        Font();

        // Destuctor
        ~Font();

        // Disable copy construction and copy assignment
        Font(const Font& other) = delete;
        Font& operator=(const Font& other) = delete;

        // Enable move construction
        Font(Font&& other) = default;

        // Loads current font from memory file
        using BufferType = std::vector<std::uint8_t>;
        void Load(const BufferType& fontData);

        // Retrieves Glyph from the loaded font set
        const Glyph* operator[](const char c) const;

        // Retrieves the loaded bitmap height in pixels
        int GetPixelHeight() const;

        // Unloads the loaded font data
        void Destroy();

    private:
        // Stores the loaded Glyph data
        std::map<char, Glyph> mGlyphs;
        // Stores the loaded pixel size
        int mPixelHeight;
};

#endif // ! _FONT_HPP_
