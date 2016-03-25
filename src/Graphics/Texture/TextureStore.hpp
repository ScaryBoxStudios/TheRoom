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
#ifndef _TEXTURESTORE_HPP_
#define _TEXTURESTORE_HPP_

#include <unordered_map>
#include <string>
#include <glad/glad.h>
#include "../Image/PixelBufferTraits.hpp"
#include "../Image/RawImageTraits.hpp"

struct TextureDescription
{
    GLuint texId;
};

class TextureStore
{
    public:
        // Constructor
        TextureStore();

        // Destructor
        ~TextureStore();

        // Disable copy construction
        TextureStore(const TextureStore& other) = delete;
        TextureStore& operator=(const TextureStore& other) = delete;

        // Enable move construction
        TextureStore(TextureStore&& other) = default;
        TextureStore& operator=(TextureStore&& other) = default;

        // Loads given texture to the gpu, returns mapping
        template <typename PixelBuffer>
        void Load(const std::string& name, const PixelBuffer& pb);

        // Retrieves a pointer to a loader texture object
        TextureDescription* operator[](const std::string& name);

        // Unloads stored textures in the store
        void Clear();

    private:
        std::unordered_map<std::string, TextureDescription> mTextures;
};

template <typename PixelBuffer>
void TextureStore::Load(const std::string& name, const PixelBuffer& pb)
{
    // Gen texture
    GLuint id;
    glGenTextures(1, &id);

    // Load data
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint format = PixelBufferTraits<PixelBuffer>::Channels(pb) == 4 ? GL_RGBA : GL_RGB;
    uint32_t width = PixelBufferTraits<PixelBuffer>::Width(pb);
    uint32_t height = PixelBufferTraits<PixelBuffer>::Height(pb);
    const GLvoid* data = PixelBufferTraits<PixelBuffer>::GetData(pb);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Store
    TextureDescription td;
    td.texId = id;
    mTextures.insert({name, td});
}

#endif // ! _TEXTURESTORE_HPP_
