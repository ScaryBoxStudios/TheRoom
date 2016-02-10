#include "ImageLoader.hpp"
#include <stdexcept>
#include <istream>
#include <streambuf>
#include "../../Util/FileLoad.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include "Png/Png.hpp"
#include "Jpeg/JpegLoader.hpp"
#include "Tga/TgaLoader.hpp"
WARN_GUARD_OFF

// Helper to access a Buffer type using an istream
template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class vectorwrapbuf : public std::basic_streambuf<CharT, TraitsT>
{
    public:
        vectorwrapbuf(std::vector<CharT> &vec)
        {
            this->setg(vec.data(), vec.data(), vec.data() + vec.size());
        }
};

auto ImageLoader::Load(const Buffer& buf, const std::string& hint) -> RawImage<Buffer>
{
    if (hint == "jpg" || hint == "jpeg")
    {
        JpegLoader jpegLoader;
        return jpegLoader.Load(buf);
    }
    else if (hint == "tga")
    {
        TgaLoader tgaLoader;
        return tgaLoader.Load(buf);
    }
    else if (hint == "png")
    {
        // Create png++ image object
        using PngPixelType = png::rgba_pixel;
        using PngBufferType = png::solid_pixel_buffer<PngPixelType>;
        png::image<PngPixelType, PngBufferType> img;

        // Pass a istream object that reads throught the buf to the read_stream func
        vectorwrapbuf<char> wb((std::vector<char>&)(buf));
        std::istream is(&wb);
        img.read_stream(is);

        // Create the RawImage object from the read png++ image
        auto& pb = img.get_pixbuf();
        return RawImage<>(
            pb.fetch_bytes(),
            {
                static_cast<uint32_t>(pb.get_width()),
                static_cast<uint32_t>(pb.get_height()),
                static_cast<uint8_t>(PngBufferType::pixel_traits_t::get_channels())
            }
        );
    }
    return RawImage<>({}, {0,0,0});
}

auto ImageLoader::LoadFile(const std::string& filename) -> RawImage<Buffer>
{
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    auto file = FileLoad<Buffer>(filename);
    if (!file)
        throw std::runtime_error("Could not find file: \n" + filename);
    return Load(*file, extension);
}
