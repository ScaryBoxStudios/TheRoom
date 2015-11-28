#ifndef _JPEG_LOADER_HPP_
#define _JPEG_LOADER_HPP_

#include <vector>
#include <string>
#include <cstdint>

class JpegLoader
{
    public:
        // Retrieves the raw pixel data from the given jpeg file data contents
        std::vector<uint8_t> Load(std::vector<uint8_t>&& fileData);

        // Retrieves the last error that the jpeg loader bumped into
        const std::string& GetLastError() const;

    private:
        std::string mLastError = "";
};

#endif
