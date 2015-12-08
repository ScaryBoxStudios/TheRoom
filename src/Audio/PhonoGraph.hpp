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
#ifndef _PHONOGRAPH_HPP_
#define _PHONOGRAPH_HPP_

#include <string>
#include <string>
#include <vector>
#include <thread>
#include <AL/Al.h>
#include <AL/Alc.h>

class PhonoGraph
{
    public:
        // Constructor
        PhonoGraph();

        // Initializes audio context
        void Init();

        // Deinitializes audio context
        void Shutdown();

        // Retrieves available playback device names
        std::vector<std::string> GetDevices();

        // Sets the given device as the current playback device
        void SetPlaybackDevice(const std::string& device);

        // Retrieves the master volume value
        float GetMasterVolume() const;

        // Sets the master volume in a range of [0-100]
        void SetMasterVolume(float vol);

        // Increases/decreases volume by relative factor
        void AdjustVolume(float percent);

        // Mutes/unmutes master playback
        void Mute(bool on);

        // Gets the mute switch state
        bool IsMuted() const;

        // Plays the given PCM audio
        template <typename T>
        void Play(T t, std::function<void()> finishCb);

    private:
        // Checks if error occured in last call
        void CheckALError();

        // Used playback device
        ALCdevice* mDevice;

        // Used audio render context
        ALCcontext* mContext;

        // Code from the last error that occured
        ALCenum mLastErrorCode;
};

#endif // ! _PHONOGRAPH_HPP_

