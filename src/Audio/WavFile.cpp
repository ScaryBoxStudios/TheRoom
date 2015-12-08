#include "WavFile.hpp"

void WavFile::Load(std::vector<std::uint8_t> f)
{
    // Parse to Wave data structures
    decltype(f)::iterator it = std::begin(f);
    // Should be equal to "RIFF"
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wDesc.riff));
    it += 4;
    // Should be equal to the size of the entire file in bytes minus 8 bytes for the two fields already parsed
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wDesc.size));
    it += 4;
    // Should be equal to "WAVE"
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wDesc.wave));
    it += 4;

    // Should be equal to "fmt " (note the space)
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wFmt.id));
    it += 4;
    // Should be equal to 16 for PCM. This is the size of the rest of the subchunk which follows this number
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wFmt.size));
    it += 4;
    // Should be equal to 1 for PCM. Other values indicate compression.
    std::copy(it, it + 2, reinterpret_cast<std::uint8_t*>(&wFmt.format));
    it += 2;
    // Mono = 1, Stereo = 2
    std::copy(it, it + 2, reinterpret_cast<std::uint8_t*>(&wFmt.channels));
    it += 2;
    // 8000, 44100, etc.
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wFmt.sampleRate));
    it += 4;
    // == SampleRate * NumChannels * BitsPerSample / 8
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wFmt.byteRate));
    it += 4;
    // == NumChannels * BitsPerSample / 8
    std::copy(it, it + 2, reinterpret_cast<std::uint8_t*>(&wFmt.blockAlign));
    it += 2;
    // 8 bits = 8, 16 bits = 16, etc.
    std::copy(it, it + 2, reinterpret_cast<std::uint8_t*>(&wFmt.bitsPerSample));
    it += 2;
    // Skip extra parameters that might exist
    if (wFmt.size != 16)
    {
        std::uint16_t extraParamSz;
        std::copy(it, it + 2, reinterpret_cast<std::uint8_t*>(&extraParamSz));
        it += 2 + extraParamSz;
    }

    // Should be equal to "data"
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wChunk.id));
    it += 4;
    // == NumSamples * NumChannels * BitsPerSample / 8 (number of bytes in data)
    std::copy(it, it + 4, reinterpret_cast<std::uint8_t*>(&wChunk.size));
    it += 4;

    // Remove wav header from data buffer and keep data
    f.erase(std::begin(f), std::begin(f) + 44);
    wChunk.data = std::move(f);
}

bool WavFile::IsPCM() const
{
    return wFmt.format == 1;
}

std::uint16_t WavFile::Channels() const
{
    return wFmt.channels;
}

std::uint32_t WavFile::SampleRate() const
{
    return wFmt.sampleRate;
}

std::uint16_t WavFile::BitsPerSample() const
{
    return wFmt.bitsPerSample;
}

std::uint32_t WavFile::DataSz() const
{
    return wChunk.size;
}

const std::uint8_t* WavFile::Data() const
{
    return wChunk.data.data();
}

