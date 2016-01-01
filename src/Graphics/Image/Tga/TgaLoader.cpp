#include "TgaLoader.hpp"
#include <array>
#include <memory>
#include <stddef.h>

enum class TgaDataType
{
    Null     = 0,  // No image data included
    Cmap     = 1,  // Uncompressed, color-mapped images
    Rgb      = 2,  // Uncompressed, true-color image
    Mono     = 3,  // Uncompressed, black-and-white image
    RleCmap  = 9,  // Run-length encoded, color-mapped image
    RleRgb   = 10, // Run-length encoded, true-color image
    RleMono  = 11, // Run-length encoded, black-and-white image
    CmpCmap  = 32, // Compressed (Huffman/Delta/RLE) color-mapped image
    CmpCmap4 = 33  // Compressed (Huffman/Delta/RLE) color-mapped four pass image
};

struct TgaHeader
{
    std::uint8_t  idLength;        // The length of a string located after the header
    std::uint8_t  colourMapType;   // Whether a color map is included
    std::uint8_t  dataTypeCode;    // Compression and color type
    std::uint16_t colourMapOrigin; // First entry index (offset into the color map table)
    std::uint16_t colourMapLength; // Color map length (number of entries)
    std::uint8_t  colourMapDepth;  // Color map entry size, in bits (number of bits per pixel)
    std::uint16_t xOrigin;         // X-origin of image (absolute coordinate of lower-left corner)
    std::uint16_t yOrigin;         // Y-origin of image (as for X-origin)
    std::uint16_t width;           // Image width
    std::uint16_t height;          // Image height
    std::uint8_t  bitsPerPixel;    // Bits per pixel [0]*
    std::uint8_t  imageDescriptor; // Image descriptor, bits 0-3 give the alpha channel depth bits 4-5 give direction
};

// [0]: When 23 or 32 the normal conventions apply. For 16 bits each color component is stored as 5 bits
// and the remaining bit is a binary alpha value. The color components are converted into single byte components
// by simply shifting each component up by 3 bits (multiply by 8)

#define PARSE_FIELD(field, offset)                      \
    std::copy(                                          \
        begin + offset,                                 \
        begin + offset + sizeof(TgaHeader::field),      \
        reinterpret_cast<std::uint8_t*>(&header.field)  \
    );

auto TgaLoader::Load(BufferType fileData) -> RawImage<BufferType>
{
    // Parse header
    TgaHeader header = {};
    const auto begin = std::begin(fileData);

    PARSE_FIELD(idLength, 0)
    PARSE_FIELD(colourMapType, 1)
    PARSE_FIELD(dataTypeCode, 2)
    PARSE_FIELD(colourMapOrigin, 3)
    PARSE_FIELD(colourMapLength, 5)
    PARSE_FIELD(colourMapDepth, 7)
    PARSE_FIELD(xOrigin, 8)
    PARSE_FIELD(yOrigin, 10)
    PARSE_FIELD(width, 12)
    PARSE_FIELD(height, 14)
    PARSE_FIELD(bitsPerPixel, 16)
    PARSE_FIELD(imageDescriptor, 17)

    if (static_cast<TgaDataType>(header.dataTypeCode) != TgaDataType::Rgb)
    {
        mLastError = "Unsupported TGA datatype";
        return RawImage<BufferType>({}, {0, 0, 0});
    }

    // Gather image info
    uint32_t width = header.width;
    uint32_t height = header.height;
    uint32_t pixelSz = header.bitsPerPixel / 8;

    // Remove read data from buffer
    fileData.erase(begin, begin + 18 + header.idLength);

    // Remove trailing data from buffer
    fileData.erase(std::begin(fileData) + width * height * pixelSz, std::end(fileData));

    // Create the ImageProperties structure for the RawImage returned
    ImageProperties imProps;
    imProps.width = width;
    imProps.height = height;
    imProps.channels = 3;

    // Return the RawImage structure
    return RawImage<>(std::move(fileData), imProps);
}
