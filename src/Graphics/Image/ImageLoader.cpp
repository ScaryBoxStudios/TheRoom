#include "ImageLoader.hpp"
#include <stdexcept>
#include "../../Util/FileLoad.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include "Png/Png.hpp"
#include "Jpeg/JpegLoader.hpp"
WARN_GUARD_OFF

auto ImageLoader::Load(const Buffer& buf, const std::string& hint) -> RawImage<Buffer>
{
    if (hint == "jpg" || hint == "jpeg")
    {
        JpegLoader jpegLoader;
        return jpegLoader.Load(buf);
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
