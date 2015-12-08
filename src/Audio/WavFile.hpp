/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _WAVFILE_HPP_
#define _WAVFILE_HPP_

#include <vector>
#include <cstdint>

class WavFile
{
    public:
        // The "RIFF" chunk descriptor
        struct WavDesc
        {
            std::uint8_t riff[4];
            std::uint32_t size;
            std::uint8_t wave[4];
        };

        // Describes the format of the sound info in the data sub-chunk
        struct WavFormat
        {
            std::uint8_t id[4];
            std::uint32_t size;
            std::uint16_t format;
            std::uint16_t channels;
            std::uint32_t sampleRate;
            std::uint32_t byteRate;
            std::uint16_t blockAlign;
            std::uint16_t bitsPerSample;
        };

        // Indicates the size of the sound info and contains the raw sound data
        struct WavChunk
        {
            std::uint8_t id[4];
            std::uint32_t size;
            std::vector<std::uint8_t> data;
        };

        // Parses wav file data into memory structs
        void Load(std::vector<std::uint8_t> fileData);

        // Shows if wav file contains PCM data
        bool IsPCM() const;

        // Returns the number of channels we have sample data for
        std::uint16_t Channels() const;

        // Returns the sample rate of the audio data
        std::uint32_t SampleRate() const;

        // Returns the number of bits per sample
        std::uint16_t BitsPerSample() const;

        // Returns the size of the audio data in bytes
        std::uint32_t DataSz() const;

        // Returns raw ptr to the audio data
        const std::uint8_t* Data() const;

    private:
        WavDesc wDesc;
        WavFormat wFmt;
        WavChunk wChunk;
};

#endif // ! _WAVFILE_HPP_

