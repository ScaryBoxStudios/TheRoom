#include "TiffLoader.hpp"
#include <istream>
#include <sstream>
#include "tiff.h"
#include "tiffio.h"
#include "tiffio.hxx"

template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class vectorwrapbuf : public std::basic_streambuf<CharT, TraitsT>
{
    public:
        using off_type = typename TraitsT::off_type;
        using pos_type = typename TraitsT::pos_type;

        vectorwrapbuf(std::vector<CharT> &vec) : mVec(vec), mCurPos(0)
        {
            this->setg(vec.data(), vec.data(), vec.data() + vec.size());
        }

        virtual pos_type seekpos(
            pos_type pos,
            std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override
        {
            (void) which;
            mCurPos = pos;
            return mCurPos;
        }

        virtual pos_type seekoff(
            off_type off,
            std::ios_base::seekdir way,
            std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override
        {
            (void) which;
            switch(way)
            {
                case std::ios_base::beg:
                    mCurPos = off;
                    break;
                case std::ios_base::cur:
                    mCurPos += off;
                    break;
                case std::ios_base::end:
                    mCurPos = mVec.size() - off;
                    break;
                default:
                    break;
            }
            return mCurPos;
        }

    private:
        std::vector<CharT>& mVec;
        std::streampos mCurPos;
};

auto TiffLoader::Load(BufferType fileData) -> RawImage<BufferType>
{
    // Trick to use a ostream to read from the vector buffer
    //vectorwrapbuf<char> wb((std::vector<char>&)(fileData));
    //std::istream istr(&wb);
    std::stringstream istr(std::string(std::begin(fileData), std::end(fileData)));

    // Open the tiff file from memory buffer
    TIFF* tif = TIFFStreamOpen("MemBuf", (std::istream*) &istr);

    // Get the image info
    uint32_t width, height;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

    // Resize the buffer to the data to be read
    BufferType imageData;
    imageData.resize(width * height * 4);

    // Read the image data
    TIFFReadRGBAImage(tif, width, height, reinterpret_cast<std::uint32_t*>(imageData.data()), 0);

    // Free resources
    TIFFClose(tif);

    // Create the ImageProperties structure for the RawImage returned
    ImageProperties imProps;
    imProps.width = width;
    imProps.height = height;
    imProps.channels = 3;

    // Return the RawImage structure
    return RawImage<>(std::move(imageData), imProps);
}
