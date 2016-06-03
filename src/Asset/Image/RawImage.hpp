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
#ifndef _RAWIMAGE_HPP_
#define _RAWIMAGE_HPP_

#include <vector>
#include <cstdint>

struct ImageProperties
{
    std::uint32_t width;
    std::uint32_t height;
    std::uint8_t channels;
};

template <typename Buffer = std::vector<std::uint8_t>>
class RawImage
{
    public:
        RawImage(Buffer buf, ImageProperties props);
        RawImage(const RawImage& other) = default;
        RawImage& operator=(const RawImage& other) = default;
        RawImage(RawImage&& other) = default;
        RawImage& operator=(RawImage&& other) = default;
        const std::uint8_t* Data() const;
        const ImageProperties& GetProperties() const;
    private:
        Buffer mData;
        ImageProperties mProps;
};

template <typename Buffer>
RawImage<Buffer>::RawImage(Buffer buf, ImageProperties props)
    : mData(buf),
      mProps(props)
{
}

template <typename Buffer>
const std::uint8_t* RawImage<Buffer>::Data() const
{
    return mData.data();
}

template <typename Buffer>
const ImageProperties& RawImage<Buffer>::GetProperties() const
{
    return mProps;
}

#endif // ! _RAWIMAGE_HPP_
