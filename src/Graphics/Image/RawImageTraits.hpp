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
#ifndef _RAWIMAGETRAITS_HPP_
#define _RAWIMAGETRAITS_HPP_

#include "PixelTraits.hpp"
#include "PixelBufferTraits.hpp"
#include "RawImage.hpp"

// Tag for the RawImage pixel
struct RawPixel {};

//--------------------------------------------------
// \brief Pixel traits class specialization
// for JpegPixel
//--------------------------------------------------
template <>
struct PixelTraits<RawPixel>
{
    static unsigned int GetChannels()
    {
        return 3;
    }

    static unsigned int GetBitDepth()
    {
        return 8;
    }
};

//--------------------------------------------------
// \brief PixelBuffer traits class specialization
// for RawImage container
//--------------------------------------------------
template <>
struct PixelBufferTraits<RawImage<>>
{
    using Pixel = RawPixel;

    static std::uint32_t Width(const RawImage<>& ri)
    {
        return ri.GetProperties().width;
    }

    static std::uint32_t Height(const RawImage<>& ri)
    {
        return ri.GetProperties().height;
    }

    static const std::uint8_t* GetData(const RawImage<>& ri)
    {
        return ri.Data();
    }
};

#endif // ! _RAWIMAGETRAITS_HPP_
