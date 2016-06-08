#include "RawImage.hpp"

RawImage::RawImage(struct image* im)
    : mImage(im)
{
}

RawImage::~RawImage()
{
    if (mImage)
        image_delete(mImage);
}

RawImage::RawImage(RawImage&& other)
    : mImage(other.mImage)
{
    other.mImage = nullptr;
}

RawImage& RawImage::operator=(RawImage&& other)
{
    mImage = other.mImage;
    other.mImage = nullptr;
    return *this;
}

const std::uint8_t* RawImage::Data() const
{
    return mImage->data;
}

std::int32_t RawImage::Width() const
{
    return mImage->width;
}

std::int32_t RawImage::Height() const
{
    return mImage->height;
}

std::int32_t RawImage::Channels() const
{
    return mImage->channels;
}
