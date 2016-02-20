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
#ifndef _AABB_HPP_
#define _AABB_HPP_

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class AABB
{
    public:
        // Default Constructor
        AABB();
        // Constructor
        AABB(glm::vec3 minPoint, glm::vec3 maxPoint);

        float MinX() const;
        float MaxX() const;
        float MinY() const;
        float MaxY() const;
        float MinZ() const;
        float MaxZ() const;

        glm::vec3 MinPoint() const;
        glm::vec3 MaxPoint() const;
        glm::vec3 Center() const;
        glm::vec3 Size() const;

        void Update(const glm::vec3& transl, const glm::vec3& scale, const glm::vec3& rotation);

    private:
        glm::vec3 localMin, cachedMin;
        glm::vec3 localMax, cachedMax;
};

bool Intersects(const glm::vec3&, const AABB& aabb);
bool Intersects(const AABB& aabb1, const AABB& aabb2);
glm::vec3 CalcCollisionResponce(const AABB& aabb1, const AABB& aabb2);

#endif // ! _AABB_HPP_
