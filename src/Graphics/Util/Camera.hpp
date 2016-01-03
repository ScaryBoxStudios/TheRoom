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
#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include <tuple>
#include <vector>
#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

///==============================================================
/// Camera
///  * Simulates movement and lookaround of a view in a 3D world
///==============================================================
class Camera
{
    public:
        /// CameraState
        struct CameraState
        {
            glm::vec3 position;
            glm::vec3 front;
            glm::vec3 up;
            float yaw;
            float pitch;
        };

        /// CameraProperties
        struct CameraProperties
        {
            float movementSpeed;
            float sensitivity;
            float pitchLim;
        };

        /// The possible movement directions of the camera
        enum class MoveDirection
        {
            Forward = 0,
            Left,
            BackWard,
            Right
        };

        /// Constructor
        Camera();

        /// Destructor
        ~Camera();

        /// Retrieves the current camera state
        const CameraState& GetCameraState() const;

        /// Sets the camera position
        void SetPos(const glm::vec3& pos);

        /// Moves the camera to the given directions
        void Move(std::vector<MoveDirection> md);

        /// Moves the camera look for the given offset
        void Look(std::tuple<float, float> lookOffset);

        /// Retrieves an interpolated copy of the camera state
        CameraState Interpolate(std::vector<MoveDirection> md, std::tuple<float, float> lookOffset, float interpolation);

    private:
        // The internal camera state
        CameraState mState;

        // The internal camera properties
        CameraProperties mProperties;

        // Returns the new camera state after performing the calculations for the given camera move and interpolation factor
        CameraState CalcMove(const CameraState& prevState, const CameraProperties& camProps, std::vector<MoveDirection> md, float interpolation);

        // Same as above but for looking around
        CameraState CalcLook(const CameraState& prevState, const CameraProperties& camProps, std::tuple<float, float> lookOffset, float interpolation);
};

#endif // ! _CAMERA_HPP_
