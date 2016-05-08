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
#ifndef _SCENE_LOADER_HPP_
#define _SCENE_LOADER_HPP_

#include <string>
#include <vector>
#include <stdint.h>
#include "../../Util/UUID.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

template<typename T>
struct Maybe
{
    T data;
    bool valid;
};

template<typename T>
bool operator==(const Maybe<T>& m1, const Maybe<T>& m2)
{
    return m1.data == m2.data;
}

namespace std
{
    template<typename T>
    std::string to_string(const Maybe<T>& m)
    {
        if(m.valid)
            return std::to_string(m.data);
        else
            return std::string("");
    }
}

//
// On the right side comments are listed the
// json keys used in the scene file format
//
struct SceneFile
{
    using Id = Maybe<std::uint32_t>;
    struct
    {
        std::string version;   // "version"
        std::string type;      // "type"
        std::string generator; // "generator"
    } metadata; // "metadata"

    struct Geometry
    {
        Id id;       // "id"
        std::string type; // "type"
        std::string url;  // "url"
    };
    std::vector<Geometry> geometries; // "geometries"

    struct Image
    {
        Id id;           // "id"
        std::string url; // "url"
    };
    std::vector<Image> images; // "images"

    // Used by Texture and Material structs
    struct Color { std::uint8_t r, g, b, a; };

    struct Texture
    {
        Id id;    // "id"
        Id image; // "image"
        enum class WrapMode
        {
            ClampToEdge,
            ClampToBorder,
            MirroredRepeat,
            Repeat
        };
        std::vector<WrapMode> wrap; // "wrap" - In s, t, r order?
        Color borderColor; // "borderColor"
    };
    std::vector<Texture> textures; // "textures"

    struct Material
    {
        Id id;              // "id"
        std::string name;   // "name"
        std::string type;   // "type" - Can be MeshPhongMaterial | ???
        Color color;        // "color"
        Color ambient;      // "ambient"
        Color emissive;     // "emissive"
        Color specular;     // "specular"
        float roughness;    // "roughness"
        float fresnel;      // "fresnel"
        float metallic;     // "metallic"
        float transparency; // "transparency"
        bool transparent;   // "transparent"
        bool wireframe;     // "wireframe"
        Id map;             // "map" - Diffuse map
        Id specMap;         // "specMap" - Specular map
        Id nmap;            // "nmap" - Normal map
    };
    std::vector<Material> materials; // "materials"

    struct Object
    {
        struct Transform
        {
            glm::vec3 position; // "position"
            glm::vec3 rotation; // "rotation"
            glm::vec3 scale;    // "scale"
        };
        Id id;               // "id"
        std::string type;    // "type" - Can be Scene|Mesh|PointLight|SpotLight|DirectionalLight
        Transform transform; // "transform"
        struct Child
        {
            Id id;       // "id"
            std::string type; // "type"
            std::string name; // "name"
            Transform transform; // "transform"
            Id geometry;   // "geometry"
            std::vector<Id> materials; // "materials"
        };
        std::vector<Child> children; // "children"
    };
    Object object; // "object"
};

class SceneLoader
{
    public:
        using Buffer = std::vector<std::uint8_t>;
        SceneFile Load(const Buffer& buf);
};

#endif // ! _SCENE_LOADER_HPP_
