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
#ifndef _SCENENODE_HPP_
#define _SCENENODE_HPP_

#include <string>
#include <vector>
#include "Transform.hpp"
#include "AABB.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

/// Scene Object Categories
enum class SceneNodeCategory
{
    Normal,
    Light,
    Invalid
};

enum class RotationAxis
{
    X,
    Y,
    Z
};

class SceneNode
{
    using ChildrenList = std::vector<SceneNode*>;

    public:
        /// Constructor
        SceneNode(
            const std::string& model,
            const std::string& uuid,
            SceneNodeCategory category,
            AABB localAABB,
            bool isCulled = false,
            SceneNode* parent = nullptr);

        /// Adds given child to children list
        void AddChild(SceneNode* const child);

        /// Removes the child with the given uuid from children
        void RemoveChild(const std::string& uuid);

        /// Move this node and optionally its children too
        void Move(const glm::vec3& pos, bool moveChildren = false);

        // Rotate this node and optionally its children
        void Rotate(RotationAxis axis, float angle, bool rotateChildren = false);

        // Scale this node and optionally its children too
        void Scale(const glm::vec3& scale, bool scaleChildren = false);

        /// Get model
        const std::string& GetModel() const;

        /// Get transformation
        Transform& GetTransformation();

        /// Get category
        SceneNodeCategory GetCategory() const;

        /// Get UUID
        const std::string& GetUUID() const;

        /// Get AABB
        AABB& GetAABB();

        /// Get culled
        bool IsCulled() const;

        /// Get a list with children nodes
        const ChildrenList& GetChildren() const;

    private:
        std::string mModel;          /// Node's model name
        Transform mTransform;        /// Node's current transformation
        SceneNodeCategory mCategory; /// Node's category
        std::string mUuid;           /// Node's unique id
        AABB mAABB;                  /// Node's AABB
        bool mCulled;                /// Is the node culled?

        SceneNode* mParent;          /// Node's parent object
        ChildrenList mChildren;      /// Node's children objects

        /// Moves all children of this node
        inline void MoveChildren(const glm::vec3& pos);

        /// Rotates all children of this node
        inline void RotateChildren(RotationAxis axis, float angle);

        /// Scales all children of this node
        inline void ScaleChildren(const glm::vec3& scale);

        /// Find if there is a child with the given uuid
        inline ChildrenList::iterator FindChildByUuid(const std::string& uuid);
}; // ! SceneNode

#endif // ! _SCENENODE_HPP_
