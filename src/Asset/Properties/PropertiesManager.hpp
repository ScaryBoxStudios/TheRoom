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
#ifndef _PROPERTIES_MANAGER_HPP_
#define _PROPERTIES_MANAGER_HPP_

#include <vector>
#include <string>
#include <memory>

#include "Properties.hpp"
#include "PropertiesLoader.hpp"

/*
 * Properties manager is responsible to manage the property system pipeline and succefully load and validate
 * properties.
 *
 * PROPERTIES SYSTEM
 *   Each physical Json file has its struct counterpart (SceneFile, MaterialFile, ModelFile). Each struct
 *   contains elements that represent objects in Json. Properties can be scattered in various files but
 *   before usage they MUST be merged into a single SceneFile.
 *
 *   In other words, you actually have ONE SceneFile which you may (or may not) divide into separate files
 *   for better organization. It's Validator's responsibility to do the error checking and ensure that all
 *   those files do not have overlapping properties (eg. multiple IDs).
 *
 * PIPELINE
 *   PropertiesManager offers a fixed pipeline to load and validate properties:
 *
 *     Parse -> Validate (File Scope) -> Merge into a single SceneFile -> Validate (Global Scope)
 *
 *   where "File Scope" means that each file is checked for errors such as incorrect types while the
 *   "Global Scope" means that the merged SceneFile is checked for errors such as multiple defined IDs
 *   or undefined ID references.
 */
class PropertiesManager
{
    public:
        using SceneFileContainer    = PropertiesLoader::OutputContainer<Properties::SceneFile>;
        using MaterialFileContainer = PropertiesLoader::OutputContainer<Properties::MaterialFile>;
        using ModelFileContainer    = PropertiesLoader::OutputContainer<Properties::ModelFile>;

        struct LoadInput
        {
            std::string id;
            std::string filename;
        };

        Properties::SceneFile Load(
            const std::vector<LoadInput>& scenes,
            const std::vector<LoadInput>& materials,
            const std::vector<LoadInput>& models);

    private:
        PropertiesManager& ParseProperties(
            const std::vector<LoadInput>& scenes,
            const std::vector<LoadInput>& materials,
            const std::vector<LoadInput>& models,
            SceneFileContainer& sceneBuf,
            MaterialFileContainer& materialBuf,
            ModelFileContainer& modelBuf);

        PropertiesManager& ValidateProperties(
                const SceneFileContainer& scenes,
                const MaterialFileContainer& materials,
                const ModelFileContainer& models);

        PropertiesManager& MergeProperties(
                Properties::SceneFile& mergedPropsBuffer,
                const SceneFileContainer& scenes,
                const MaterialFileContainer& materials,
                const ModelFileContainer& models);

        void ValidateMergedProperties(Properties::SceneFile& mergedProps);
};

#endif // ! _PROPERTIES_MANAGER_HPP_
