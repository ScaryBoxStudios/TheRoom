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
#ifndef _MODELSTORE_HPP_
#define _MODELSTORE_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <glad/glad.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Geometry.hpp"
#include "Material.hpp"

// MeshDescription
struct MeshDescription
{
    GLuint vaoId;
    GLuint vboId;
    GLuint eboId;
    GLsizei numIndices;
};

// ModelDescription
struct ModelDescription
{
    std::vector<MeshDescription> meshes;
    Material material;
    AABB localAABB;
};

// ModelStore
class ModelStore
{
    public:
        // Constructor
        ModelStore();

        // Destructor
        ~ModelStore();

        // Disable copy construction
        ModelStore(const ModelStore& other) = delete;
        ModelStore& operator=(const ModelStore& other) = delete;

        // Enable move construction
        ModelStore(ModelStore&& other) = default;
        ModelStore& operator=(ModelStore&& other) = default;

        // Loads given data into the GPU
        void Load(const std::string& name, const ModelData& data);

        // Retrieves pointer a loaded model object
        ModelDescription* operator[](const std::string& name);

        // Unloads the stored models in the store
        void Clear();

    private:
        std::unordered_map<std::string, ModelDescription> mModels;
};

#endif // ! _MODELSTORE_HPP_
