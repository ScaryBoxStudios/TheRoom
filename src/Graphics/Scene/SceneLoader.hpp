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

//
// On the right side comments are listed the
// json keys used in the scene file format
//
struct SceneFile
{
    struct
    {
        std::string version;   // "version"
        std::string type;      // "type"
        std::string generator; // "generator"
    } metadata; // "metadata"

    struct Geometry
    {
        rUUID uuid;       // "uuid"
        std::string type; // "type"
    };
    std::vector<Geometry> geometries; // "geometries"

    struct Image
    {
        rUUID uuid;      // "uuid"
        std::string url; // "url"
    };
    std::vector<Image> images; // "images"

    // Used by Texture and Material structs
    struct Color { std::uint8_t r, g, b, a; };

    struct Texture
    {
        rUUID uuid;  // "uuid"
        rUUID image; // "image"
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
        rUUID uuid;       // "uuid"
        std::string name; // "name"
        std::string type; // "type" - Can be MeshPhongMaterial | ???
        Color color;      // "color"
        Color ambient;    // "ambient"
        Color emissive;   // "emissive"
        Color specular;   // "specular"
        float shininess;  // "shininess"
        float opacity;    // "opacity"
        bool transparent; // "transparent"
        bool wireframe;   // "wireframe"
        rUUID map;        // "map" - Diffuse map
        rUUID specMap;    // "specMap" - Specular map
    };
    std::vector<Material> materials; // "materials"

    struct Object
    {
        rUUID uuid;       // "uuid"
        std::string type; // "type" - Can be Scene|Mesh|PointLight|SpotLight|DirectionalLight
        glm::mat4 matrix; // "matrix"
        struct Child
        {
            rUUID uuid;       // "uuid"
            std::string type; // "type"
            std::string name; // "name"
            glm::mat4 matrix; // "matrix"
            rUUID geometry;   // "geometry"
            rUUID material;   // "material"
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
