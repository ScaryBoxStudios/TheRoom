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
#ifndef _TRANSFORM_HPP_
#define _TRANSFORM_HPP_

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class Transform
{
    public:
        // Constructor
        Transform();

        // Retrieves the current transformation matrix
        const glm::mat4& Get() const;

        // Updates the current transformation matrix
        void Set(const glm::mat4& mat);

        // Retrieves the current local position vector
        const glm::vec3& GetPosition() const;

        // Retrieves the current local scaling vector
        const glm::vec3& GetScale() const;

        // Retrieves the current rotation vector
        const glm::vec3 GetRotation() const;

        // Moves relatively to the current position
        void Move(const glm::vec3& pos);

        // Rotates relatively to the current X rotation
        void RotateX(float angle);

        // Rotates relatively to the current Y rotation
        void RotateY(float angle);

        // Rotates relatively to the current Z rotation
        void RotateZ(float angle);

        // Scales relatively to the current scale
        void Scale(const glm::vec3& scale);

        // Updates the interpolation state variables
        void Update();

        // Calculates interpolated translation matrix
        glm::mat4 GetInterpolated(float interpolation) const;

    private:
        // Current cached transformation matrix
        glm::mat4 mTransform;

        // Transform position
        glm::vec3 mPosition;

        // Transform rotation
        float mYaw, mPitch, mRoll;

        // Transform scale
        glm::vec3 mScale;

        // Stored for calculating interpolation states
        glm::mat4 mOldTrans;
};

#endif // ! _TRANSFORM_HPP_

