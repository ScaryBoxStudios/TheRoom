#include "PhonoGraph.hpp"
#include <string.h>

PhonoGraph::PhonoGraph() :
    mDevice(nullptr),
    mContext(nullptr)
{
}

template <typename T>
void PhonoGraph::Play(T sound, std::function<void()> finishCb)
{
    // Create audio source
    ALuint source;
    alGenSources(1, &source);

    // Set its properties
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    // Generate audio buffer object
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // Load pcm data into buffer
    auto alFmtFromInfo = [](uint16_t channels, uint16_t bitsPerSample)
    {
        bool stereo = (channels > 1);

        switch (bitsPerSample)
        {
            case 16:
                if (stereo)
                    return AL_FORMAT_STEREO16;
                else
                    return AL_FORMAT_MONO16;
            case 8:
                if (stereo)
                    return AL_FORMAT_STEREO8;
                else
                    return AL_FORMAT_MONO8;
            default:
                return -1;
        }
    };
    alBufferData(buffer, alFmtFromInfo(sound.Channels(), sound.BitsPerSample()), sound.Data(), sound.DataSz(), sound.SampleRate());

    // Bind the source with its buffer
    alSourcei(source, AL_BUFFER, buffer);

    // Play
    alSourcePlay(source);

    // Cleanup thread
    std::thread t(
        [source, buffer, finishCb]()
        {
            // Wait till playing finishes
            ALint sourceState;
            alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
            while (sourceState == AL_PLAYING)
            {
                alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            // Release resources
            alDeleteSources(1, &source);
            alDeleteBuffers(1, &buffer);

            // Call finish callback
            finishCb();
        }
    );
    t.detach();
}

void PhonoGraph::Init()
{
    mDevice = alcOpenDevice(nullptr);
    if (!mDevice)
        CheckALError();

    // Create audio render context, and use it
    mContext = alcCreateContext(mDevice, nullptr);
    if (!alcMakeContextCurrent(mContext))
        CheckALError();
}

void PhonoGraph::Shutdown()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(mContext);
    alcCloseDevice(mDevice);
}

std::vector<std::string> PhonoGraph::GetDevices()
{
    // Names of the playback devices available
    std::vector<std::string> playbackDevices;

    // Check if device enumeration is possible
    ALboolean enumeration;
    enumeration = alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE) // When this happens listing audio devices only return the default device
        return playbackDevices;
    else
    {
        // String devices are separated with a null char and the list is terminated by two null chars
        const ALCchar* devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
        const ALCchar* device = devices;
        const ALCchar* next = devices + 1;
        size_t len = 0;

        while (device && *device != '\0' && *next != '\0')
        {
            playbackDevices.push_back(device);
            len = strlen(device);
            device += (len + 1);
            next += (len + 2);
        }
    }
    return playbackDevices;
}

void PhonoGraph::CheckALError()
{
    ALCenum error;
    error = alGetError();
    if (error != AL_NO_ERROR)
        mLastErrorCode = error;
}

