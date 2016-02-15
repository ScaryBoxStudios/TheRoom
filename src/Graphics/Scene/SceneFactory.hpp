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
#ifndef _SCENEFACTORY_HPP_
#define _SCENEFACTORY_HPP_

#include <memory>
#include "Scene.hpp"
#include "SceneLoader.hpp"
#include "../Texture/TextureStore.hpp"
#include "../Geometry/ModelStore.hpp"
#include "../Material/MaterialStore.hpp"
#include "../../Game/Screen.hpp"

class SceneFactory
{
    public:
        // Constructor
        SceneFactory(TextureStore* tStore, ModelStore* mdlStore, MaterialStore* matStore, ScreenContext::FileDataCache* fdc);

        // Creates a scene from a given SceneFile struct
        std::unique_ptr<Scene> CreateFromSceneFile(const SceneFile& sceneFile);

    private:
        // The stores that will be needed by the factory
        TextureStore*  mTextureStore;
        ModelStore*    mModelStore;
        MaterialStore* mMaterialStore;
        ScreenContext::FileDataCache* mFileDataCache;

        // Loads the textures
        void LoadTextures(const std::vector<SceneFile::Texture>& textures, const std::vector<SceneFile::Image>& images);

        // Loads the materials
        void LoadMaterials(const std::vector<SceneFile::Material>& materials);

        // Load geometries
        void LoadGeometries(const std::vector<SceneFile::Geometry>& geometries);

        // Create scene
        void BakeScene(Scene* const sceneToBake, const std::vector<SceneFile::Object::Child>& children);
};

#endif // ! _SCENEFACTORY_HPP_