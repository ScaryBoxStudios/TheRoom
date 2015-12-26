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
#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <map>
#include <unordered_map>
#include <vector>
#include <string>

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

#include "SceneObject.hpp"

class Scene
{
    public:
        /// Adds new object to scene
        void AddObject(
            const std::string& name,         /// Object's name, unique for every scene object
            const std::string& modelName,    /// Model's name, can be the same for many objects
            SceneObjCategory category,       /// Object's category
            const std::string& parent = ""); /// Parent object's name (optional)

        /// Removes an object from the scene
        void RemoveObject(const std::string& name);

        /// Attach a child to a parent
        void AttachObject(const std::string& child, const std::string& parent);

        /// Detach a child from a parent
        void DetachObject(const std::string& child, const std::string& parent);

        /// Moves relatively to the current position
        void Move(const std::string& objName, const glm::vec3& pos);

        /// Rotates relatively to the current X rotation
        void RotateX(const std::string& objName, float angle);

        /// Rotates relatively to the current Y rotation
        void RotateY(const std::string& objName, float angle);

        /// Rotates relatively to the current Z rotation
        void RotateZ(const std::string& objName, float angle);

        /// Scales relatively to the current scale
        void Scale(const std::string& objName, const glm::vec3& scale);

        /// Get scene's objects
        std::map<std::string, SceneObject>& GetObjects();

        /// Get category map for renderer
        std::unordered_map<SceneObjCategory, std::vector<std::map<std::string, SceneObject>::iterator>>&
            GetCategories();

    private:
        std::map<std::string, SceneObject> mObjects; /// Scene's objects

        // TODO: Find a better way to sort objects by category
        // Problem: The problem is that scene owns the data but many other modules
        // need them in different formats. For example renderer wants them sorted
        // by category
        std::unordered_map<SceneObjCategory, std::vector<std::map<std::string, SceneObject>::iterator>> mCategories;
}; // ! Scene

#endif // ! _SCENE_HPP_