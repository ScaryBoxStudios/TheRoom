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
#ifndef _MATERIALSTORE_HPP_
#define _MATERIALSTORE_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <glad/glad.h>
#include "../../Asset/Material/Material.hpp"

struct MaterialDescription
{
    GLuint matIndex;
    Material material;
};

class MaterialStore
{
    public:
        // Constructor
        MaterialStore();

        // Destructor
        ~MaterialStore();

        // Disable copy construction
        MaterialStore(const MaterialStore&) = delete;
        MaterialStore& operator=(const MaterialStore&) = delete;

        // Enable move construction
        MaterialStore(MaterialStore&&) = default;
        MaterialStore& operator=(MaterialStore&&) = default;

        // Load a material to store
        void Load(const std::string& name, const Material& material);

        // Retrieves a pointer to a loaded material object
        MaterialDescription* operator[](const std::string& name);

        // Unloads every material in the store
        void Clear();

        // Retrieves the GPU data id
        GLuint DataId() const;

    private:
        GLuint mUBO;
        std::unordered_map<std::string, std::size_t> mMaterials;
        std::vector<MaterialDescription> mMaterialDescs;
};

#endif // ! _MATERIALSTORE_HPP_
