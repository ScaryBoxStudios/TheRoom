#include "PngLoader.hpp"
#include <array>
#include <memory>
#include <png.h>

auto PngLoader::Load(BufferType fileData) -> RawImage<BufferType>
{
    // The data that will be returned
    BufferType rawData;

    if (png_sig_cmp(static_cast<png_byte*>(rawData.data()), 0, 8))
    {
        mLastError = "Incorrect png header";
        return RawImage<>(std::move(rawData), {0,0,0});
    }

    auto del = [](png_struct* p){ png_destroy_read_struct(&p, nullptr, nullptr); };
    std::unique_ptr<png_struct, decltype(del)> png(
            png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr),
            del);

    auto del2 = [](png_info* p){ png_destroy_read_struct(nullptr, &p, nullptr); };
    std::unique_ptr<png_info, decltype(del2)> info(
            png_create_info_struct(png.get()),
            del2);

    auto del3 = [](png_info* p){ png_destroy_read_struct(nullptr, nullptr, &p); };
    std::unique_ptr<png_info, decltype(del3)> endInfo(
            png_create_info_struct(png.get()),
            del3);

    using CbData = std::tuple<const BufferType&, BufferType::const_iterator>;
    CbData cbData = std::make_tuple(fileData, std::begin(fileData));

    png_set_read_fn(png.get(), static_cast<void*>(&cbData),
        [](png_struct* png, png_byte* data, png_size_t length)
        {
            CbData* cbData = static_cast<CbData*>(png_get_io_ptr(png));
            auto it = std::get<1>(*cbData);
            std::copy(it, it + length, data);
            it += length;
        }
    );

    png_set_sig_bytes(png.get(), 8);
    png_read_info(png.get(), info.get());

    uint32_t width = png_get_image_width(png.get(), info.get());
    uint32_t height = png_get_image_height(png.get(), info.get());

    // Bits per CHANNEL not per pixel
    uint32_t bitDepth = png_get_bit_depth(png.get(), info.get());
    uint8_t channels = png_get_channels(png.get(), info.get());
    //uint32_t colorType = png_get_color_type(png.get(), info.get());

    std::vector<png_byte*> rowPtrs(height);
    rawData.resize(width * height * bitDepth * channels / 8);

    //const unsigned int stride = width * bitDepth * channels / 8;
    const size_t stride = png_get_rowbytes(png.get(), info.get());
    for (size_t i = 0; i < height; ++i)
    {
        size_t q = (height - i - 1) * stride;
        rowPtrs[i] = rawData.data()  + q;
    }
    png_read_image(png.get(), rowPtrs.data());

    // Create the ImageProperties structure for the RawImage returned
    ImageProperties imProps;
    imProps.width = width;
    imProps.height = height;
    imProps.channels = channels;

    return RawImage<>(std::move(rawData), imProps);
}
