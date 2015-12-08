#include "OggFile.hpp"
#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>
#include <ogg/os_types.h>
#include <ogg/ogg.h>

void OggFile::Load(std::vector<std::uint8_t> f)
{
    // Callback data struct
    struct CbData
    {
        std::vector<std::uint8_t>::iterator begin;
        std::vector<std::uint8_t>::iterator cur;
        size_t size;
    };

    // Initialize callback data
    CbData cbData;
    cbData.begin = std::begin(f);
    cbData.cur = std::begin(f);
    cbData.size = f.size();

    // Setup custom processing callbacks
    ov_callbacks callbacks;
    // Feeds vorbis decoder with data from the memory buffer
    callbacks.read_func = [](
        void* dest,      // Ptr to the data that vorbis file needs (to be feeded)
        size_t bSize,    // Byte size on this particular system
        size_t szToRead, // Maximum number of items to be read
        void* cbData     // A pointer to the callback data
    ) -> size_t
    {
        CbData* cbd = reinterpret_cast<CbData*>(cbData);

        size_t spaceToEOF = cbd->size - (cbd->cur - cbd->begin);
        size_t actualSizeToRead = 0;
        if ((szToRead * bSize) < spaceToEOF)
            actualSizeToRead = (szToRead * bSize);
        else
            actualSizeToRead = spaceToEOF;

        std::copy(cbd->cur, cbd->cur + actualSizeToRead, reinterpret_cast<std::uint8_t*>(dest));
        cbd->cur += actualSizeToRead;
        return actualSizeToRead;
    };
    callbacks.seek_func = [](void* cbData, ogg_int64_t offset, int type) -> int
    {
        CbData* cbd = reinterpret_cast<CbData*>(cbData);
        switch (type)
        {
            case SEEK_CUR:
            {
                cbd->cur += offset;
                break;
            }
            case SEEK_END:
                cbd->cur = cbd->begin + cbd->size - offset;
                break;
            case SEEK_SET:
                cbd->cur = cbd->begin + offset;
                break;
            default:
                return -1;
        }

        // Lower limit
        if (cbd->cur < cbd->begin)
        {
            cbd->cur = cbd->begin;
            return -1;
        }

        // Upper limit
        if (cbd->cur > cbd->begin + cbd->size)
        {
            cbd->cur = cbd->begin + cbd->size;
            return -1;
        }
        return 0;
    };
    callbacks.tell_func = [](void* cbData) -> long
    {
        CbData* cbd = reinterpret_cast<CbData*>(cbData);
        return (cbd->cur - cbd->begin);
    };
    callbacks.close_func = [](void*) -> int { return 0; };

    // Begin decompression
    OggVorbis_File oggFile = {};
    ov_open_callbacks(reinterpret_cast<void*>(&cbData), &oggFile, nullptr, 0, callbacks);

    // Gather audio info
    vorbis_info* vi = ov_info(&oggFile, -1);
    mChannels = vi->channels;
    mSampleRate = vi->rate;
    mBitsPerSample = 16;
    mDataSz = ov_pcm_total(&oggFile, -1) * mChannels * 2;

    // Pre-resize destination buffer
    std::vector<uint8_t> pcmData;
    pcmData.resize(mDataSz);
    std::uint8_t* pData = pcmData.data();

    // Currently read bytes
    unsigned long bytes = 0;

    // Decode data
    while (bytes < mDataSz)
    {
        int bitStream;
        long r = ov_read(&oggFile, reinterpret_cast<char*>(pData + bytes), mDataSz - bytes, 0, 2, 1, &bitStream);
        if (r > 0)
            bytes += r;
        else
        {
            // Error?
        }
    }

    // Move pcm data to the class member
    mData = std::move(pcmData);

    // Release decompression object
    ov_clear(&oggFile);
}

std::uint16_t OggFile::Channels() const
{
    return mChannels;
}

std::uint32_t OggFile::SampleRate() const
{
    return mSampleRate;
}

std::uint16_t OggFile::BitsPerSample() const
{
    return mBitsPerSample;
}

std::uint32_t OggFile::DataSz() const
{
    return mDataSz;
}

const std::uint8_t* OggFile::Data() const
{
    return mData.data();
}

