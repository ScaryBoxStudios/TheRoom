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
#ifndef _RENDERFORM_CREATOR_HPP_
#define _RENDERFORM_CREATOR_HPP_

#include <memory>
#include <unordered_map>
#include "Scene.hpp"
#include "../Geometry/ModelStore.hpp"
#include "../Material/MaterialStore.hpp"

class RenderformCreator
{
    public:
        struct Mesh
        {
            SceneNode*  node;
            std::string modelName;
            Transform* transformation;
            GLuint    vaoId,
                      eboId;
            GLsizei   numIndices;
        };

        struct Material
        {
            GLuint diffTexId,
                   specTexId,
                   nmapTexId,
                   matIndex;
            bool   useNormalMap;
            std::vector<Mesh> meshes;
        };

        using Renderform = std::unordered_map<std::string, Material>;

        // Constructor
        RenderformCreator(ModelStore* modelStore, MaterialStore* matStore);

        // Update the render form using scene's updates
        void Update(const Scene::Updates& sceneUpdates);

        // Retrieve the renderform
        const Renderform& GetRenderform() const;

    private:
        Renderform     mRenderform;    // The scene's element sorted to a render friendly way
        MaterialStore* mMaterialStore; // Material Store
        ModelStore*    mModelStore;    // Model Store

        // Parse added-node updates
        void ParseAddNodeUpdates(const std::vector<SceneNode*>& added);

        // Parse deleted-node updates
        void ParseDeleteNodeUpdates(const std::vector<std::unique_ptr<SceneNode>>& deleted);
};

#endif // ! _RENDERFORM_CREATOR_HPP_
