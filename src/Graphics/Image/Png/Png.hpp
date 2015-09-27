#ifndef _PNG_IMPL_HPP_
#define _PNG_IMPL_HPP_

#include <cstdint>
#include "../PixelTraits.hpp"
#include "../PixelBufferTraits.hpp"
#include <png.hpp>

//--------------------------------------------------
// \brief Pixel traits class specialization
// for png::rgba_pixel
//--------------------------------------------------
template <>
struct PixelTraits<png::rgba_pixel>
{
    static unsigned int GetChannels()
    {
        return 4;
    }

    static unsigned int GetBitDepth()
    {
        return 8;
    }
};

//--------------------------------------------------
// \brief PixelBuffer traits class specialization
// for png::pixel_buffer of Png++ library
//--------------------------------------------------
template <typename PixelType>
using SolidBuffer = png::solid_pixel_buffer<PixelType>;

template <typename PixelType>
struct PixelBufferTraits<SolidBuffer<PixelType>>
{
    using Pixel = PixelType;

    static std::size_t Width(SolidBuffer<PixelType>& pb)
    {
        return pb.get_width();
    }

    static std::size_t Height(SolidBuffer<PixelType>& pb)
    {
        return pb.get_height();
    }

    static const std::uint8_t* GetData(SolidBuffer<PixelType>& pb)
    {
        return pb.get_bytes().data();
    }
};

#endif
