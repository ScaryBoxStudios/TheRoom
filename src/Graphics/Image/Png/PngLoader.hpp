#ifndef _PNG_LOADER_HPP_
#define _PNG_LOADER_HPP_

#include <vector>
#include <string>
#include <cstdint>

class PngLoader
{
    public:
        // Retrieves the raw pixel data from the given png file data contents
        std::vector<uint8_t> Load(std::vector<uint8_t>&& fileData);

        // Retrieves the last error that the png loader bumped into
        const std::string& GetLastError() const;

    private:
        std::string mLastError = "";
};

#endif
