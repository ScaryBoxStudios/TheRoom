#include "JpegLoader.hpp"
#include <array>
#include <memory>
#include "jpeglib.h"

std::vector<uint8_t> JpegLoader::Load(std::vector<uint8_t>&& fileData)
{
    // Make a holder for our file data
    std::vector<uint8_t> imgData = fileData;

    // The data that will be returned
    std::vector<uint8_t> rawData;

    // Create decompress struct
    jpeg_decompress_struct cinfo;
    jpeg_create_decompress(&cinfo);

    // Set memory buffer as source
    jpeg_mem_src(&cinfo, imgData.data(), imgData.size());
    if (!jpeg_read_header(&cinfo, TRUE))
    {
        mLastError = "Incorrect jpeg header";
        return rawData;
    }

    // Populate the decompress structure
    jpeg_start_decompress(&cinfo);

    // Gather image info
    uint32_t width = cinfo.output_width;
    uint32_t height = cinfo.output_height;
    uint32_t pixelSz = cinfo.output_components;
    //uint32_t stride = width * pixelSz;

    // Resize the ouput buffer
    rawData.resize(width * height * pixelSz);

    // Ptr to the start of the output buffer
    uint8_t* bufP = rawData.data();

    /*
    // Read by lines
    int nSamples;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        nSamples = jpeg_read_scanlines(&cinfo, (JSAMPARRAY) &bufP, 1);
        bufP += nSamples * cinfo.image_width * cinfo.num_components;
    }
     */

    // Read whole file at once
    jpeg_read_raw_data(&cinfo, (JSAMPIMAGE) &bufP, height);

    // Release all internal buffers used by jpeg_start_decompress
    jpeg_finish_decompress(&cinfo);

    // Destroy decompress struct
    jpeg_destroy_decompress(&cinfo);

    return rawData;
}
