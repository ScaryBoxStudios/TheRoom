#include "TgaLoader.hpp"
#include <array>
#include <memory>

auto TgaLoader::Load(BufferType fileData) -> RawImage<BufferType>
{
    // The data that will be returned
    BufferType rawData;

    bool g00d = true;
    if (!g00d)
    {
        mLastError = "Incorrect tga header";
        return RawImage<>(std::move(rawData), {0,0,0});
    }

    // Gather image info
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t pixelSz = 0;

    // Resize the ouput buffer
    rawData.resize(width * height * pixelSz);

    // Ptr to the start of the output buffer
    uint8_t* bufP = rawData.data();

    // Read

    // Create the ImageProperties structure for the RawImage returned
    ImageProperties imProps;
    imProps.width = width;
    imProps.height = height;
    imProps.channels = 3;

    // Return the RawImage structure
    return RawImage<>(std::move(rawData), imProps);
}
