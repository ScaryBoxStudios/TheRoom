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

#include <string>
#include <map>
#include <memory>
#include "SceneNode.hpp"

class Scene
{
    //                           UUID               Unique Ptr to node
    using NodeBank    = std::map<std::string,       std::unique_ptr<SceneNode>>;
    using PointLights = std::vector<SceneNode*>;

    public:
        struct Updates
        {
            std::vector<SceneNode*> newNodes;
            std::vector<std::unique_ptr<SceneNode>> deletedNodes;
        };

        /// Constructor
        Scene();

        /// Creates node and adds it to NodeBank
        SceneNode* CreateNode(
            const std::string& model,
            const std::vector<std::string>& material,
            const std::string& uuid,
            Category category,
            const AABB& initAABB,
            bool isCulled = false);

        /// Deletes a node
        void DeleteNode(const std::string& uuid, bool deleteChildren = false);
        void DeleteNode(SceneNode* const node, bool deleteChildren = false);

        /// Attach child to parent
        void AttachToParent(SceneNode* child, const std::string& parentUuid);

        /// Detach child from parent
        void DetachFromParent(const std::string& childUuid, const std::string& parentUuid);

        /// Retrieves the updates and clears the update vectors
        Updates PullUpdates();

        /// Move the node and optionally its children too
        void Move(const std::string& uuid, const glm::vec3& pos, bool moveChildren = false);
        void Move(SceneNode* const node, const glm::vec3& pos, bool moveChildren = false);

        /// Rotate the node and optionally its children
        void Rotate(const std::string& uuid, RotationAxis axis, float angle, bool rotateChildren = false);
        void Rotate(SceneNode* const node, RotationAxis axis, float angle, bool rotateChildren = false);

        /// Scale the node and optionally its children too
        void Scale(const std::string& uuid, const glm::vec3& scale, bool scaleChildren = false);
        void Scale(SceneNode* const node, const glm::vec3& scale, bool scaleChildren = false);

        /// Get all nodes in their container
        const NodeBank& GetNodes() const;

        /// Get all scene's lights
        const PointLights& GetLights() const;

        /// Finds the node with the given uuid
        SceneNode* FindNodeByUuid(const std::string& uuid);

    private:
        SceneNode*  mRootNode; /// Scene's root node
        NodeBank    mNodes;    /// All nodes
        PointLights mLights;   /// Scene's point lights
        Updates     mUpdates;  /// Scene's updates (basically a diff)

        /// Clear updates
        void ClearUpdates();

}; // ! Scene

#endif // ! _SCENE_HPP_
