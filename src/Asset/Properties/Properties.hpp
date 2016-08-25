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
#ifndef _PROPERTIES_HPP_
#define _PROPERTIES_HPP_

#include <stdint.h>
#include <string>
#include <vector>
#include "../../Util/Maybe.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

namespace Properties
{
    using Id = Maybe<std::string>;

    struct Color
    {
        std::uint8_t r = 0;
        std::uint8_t g = 0;
        std::uint8_t b = 0;
        std::uint8_t a = 0;
    };

    struct Geometry
    {
        Id id;            // "id"
        std::string name; // "name"
        std::string url;  // "url"
    };

    struct Texture
    {
        Id id;           // "id"
        std::string url; // "url"
    };

    struct Material
    {
        Id id;                      // "id"
        Id dmap;                    // "dmap"
        Id smap;                    // "smap"
        Id nmap;                    // "nmap"
        std::string name;           // "name"
        Color color;                // "color"
        Color emissive;             // "emissive"
        float roughness    = 0.1f;  // "roughness"
        float reflectivity = 0.02f; // "reflectivity"
        float metallic     = 0;     // "metallic"
        float transparency = 0;     // "transparency"
        bool  wireframe    = false; // "wireframe"
    };

    struct Transform
    {
        glm::vec3 position; // "position"
        glm::vec3 rotation; // "rotation"
        glm::vec3 scale;    // "scale"
    };

    struct Model
    {
        Id id;                     // "id"
        Id geometry;               // "geometry"
        std::string name;          // "name"
        Transform transform;       // "transform"
        std::vector<Id> materials; // "materials"
    };

    struct ModelGroup
    {
        Id id;                     // "id"
        std::vector<Model> models; // "models"
        Transform transform;       // "transform"
    };

    struct Scene
    {
        std::vector<Id> models;      // "models"
        std::vector<Id> modelGroups; // "modelGroups"
        std::vector<Id> pointLights; // "pointLights"
    };

    // TODO: Remove them from Properties namespace during integration
    struct MaterialFile
    {
        std::vector<Properties::Texture> textures;   // "textures"
        std::vector<Properties::Material> materials; // "materials"
    };

    struct ModelFile
    {
        Id id;                                        // "groupId"
        std::vector<Properties::Geometry> geometries; // "geometries"
        std::vector<Properties::Model> models;        // "models"
    };

    struct SceneFile
    {
        MaterialFile extraMaterials; // "extraMaterials"
        ModelFile extraModels;       // "extraModels"
        Properties::Scene scene;     // "scene"
    };

    template<typename T>
    void print(const T& v);
    template<typename T>
    void print(const T& v, const std::string& str);
}

#endif // ! _PROPERTIES_HPP_
