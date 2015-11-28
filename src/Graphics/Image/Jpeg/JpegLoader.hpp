#ifndef _JPEG_LOADER_HPP_
#define _JPEG_LOADER_HPP_

#include <string>
#include <vector>
#include <cstdint>
#include "../RawImage.hpp"

class JpegLoader
{
    public:
        // The buffer type used by the loader
        using BufferType = std::vector<uint8_t>;

        // Retrieves the raw pixel data from the given jpeg file data contents
        RawImage<BufferType> Load(BufferType fileData);

        // Retrieves the last error that the jpeg loader bumped into
        const std::string& GetLastError() const;

    private:
        std::string mLastError = "";
};

#endif
