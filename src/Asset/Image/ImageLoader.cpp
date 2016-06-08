#include "ImageLoader.hpp"
#include <stdexcept>
#include <assets/assetload.h>

auto ImageLoader::Load(const Buffer& buf, const std::string& hint) -> RawImage
{
    struct image* im = image_from_mem_buf(buf.data(), buf.size(), hint.c_str());
    return RawImage(im);
}

auto ImageLoader::LoadFile(const std::string& filename) -> RawImage
{
    struct image* im = image_from_file(filename.c_str());
    if (!im)
        throw std::runtime_error("Could not load file: \n" + filename);
    return RawImage(im);
}
