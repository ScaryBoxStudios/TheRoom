#ifndef _TGA_LOADER_HPP_
#define _TGA_LOADER_HPP_

#include <string>
#include <vector>
#include <cstdint>
#include "../RawImage.hpp"

class TgaLoader
{
    public:
        // The buffer type used by the loader
        using BufferType = std::vector<uint8_t>;

        // Retrieves the raw pixel data from the given tga file data contents
        RawImage<BufferType> Load(BufferType fileData);

        // Retrieves the last error that the tga loader bumped into
        const std::string& GetLastError() const;

    private:
        std::string mLastError = "";
};

#endif
