#ifndef _PNG_IMPL_HPP_
#define _PNG_IMPL_HPP_

#include <cstdint>
#include "../PixelTraits.hpp"
#include "../PixelBufferTraits.hpp"
#include <png++/png.hpp>

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

    static std::uint32_t Width(SolidBuffer<PixelType>& pb)
    {
        return pb.get_width();
    }

    static std::uint32_t Height(SolidBuffer<PixelType>& pb)
    {
        return pb.get_height();
    }

    static const std::uint8_t* GetData(SolidBuffer<PixelType>& pb)
    {
        return pb.get_bytes().data();
    }

    static std::uint8_t Channels(SolidBuffer<PixelType>& pb)
    {
        (void) pb;
        return SolidBuffer<Pixel>::pixel_traits_t::get_channels();
    }

    static std::uint8_t BitDepth(SolidBuffer<PixelType>& pb)
    {
        (void) pb;
        return SolidBuffer<Pixel>::pixel_traits_t::get_bit_depth();
    }
};

#endif
