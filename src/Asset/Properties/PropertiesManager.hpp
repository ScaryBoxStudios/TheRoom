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
