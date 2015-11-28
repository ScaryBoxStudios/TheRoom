#include "JpegLoader.hpp"
#include <array>
#include <memory>
#include "jpeglib.h"

auto JpegLoader::Load(BufferType fileData) -> RawImage<BufferType>
{
    // The data that will be returned
    BufferType rawData;

    // Create decompress struct
    jpeg_decompress_struct cinfo;

    // Set the error mechanism
    jpeg_error_mgr errMgr;
    cinfo.err = jpeg_std_error(&errMgr);

    // Initialize decompression object
    jpeg_create_decompress(&cinfo);

    // Set memory buffer as source
    jpeg_mem_src(&cinfo, fileData.data(), static_cast<unsigned long>(fileData.size()));
    if (!jpeg_read_header(&cinfo, TRUE))
    {
        mLastError = "Incorrect jpeg header";
        return RawImage<>(std::move(rawData), {0,0,0});
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

    // Read by lines
    int nSamples;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        nSamples = jpeg_read_scanlines(&cinfo, (JSAMPARRAY) &bufP, 1);
        bufP += nSamples * cinfo.image_width * cinfo.num_components;
    }

    // Release all internal buffers used by jpeg_start_decompress
    jpeg_finish_decompress(&cinfo);

    // Destroy decompress struct
    jpeg_destroy_decompress(&cinfo);

    // Create the ImageProperties structure for the RawImage returned
    ImageProperties imProps;
    imProps.width = width;
    imProps.height = height;
    imProps.channels = 3;

    // Return the RawImage structure
    return RawImage<>(std::move(rawData), imProps);
}
