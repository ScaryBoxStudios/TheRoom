#ifndef _PNG_LOADER_HPP_
#define _PNG_LOADER_HPP_

#include <vector>
#include <string>
#include <cstdint>
#include "../RawImage.hpp"

class PngLoader
{
    public:
        // The buffer type used by the loader
        using BufferType = std::vector<uint8_t>;

        // Retrieves the raw pixel data from the given png file data contents
        RawImage<BufferType> Load(BufferType fileData);

        // Retrieves the last error that the png loader bumped into
        const std::string& GetLastError() const;

    private:
        std::string mLastError = "";
};

#endif
