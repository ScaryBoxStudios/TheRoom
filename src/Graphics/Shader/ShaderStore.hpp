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
#ifndef _SHADERSTORE_HPP_
#define _SHADERSTORE_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <glad/glad.h>
#include <GL/gl.h>

class ShaderStore
{
    public:
        enum class ShaderType
        {
            Vertex = GL_VERTEX_SHADER,
            Fragment = GL_FRAGMENT_SHADER
        };

        // Constructor
        ShaderStore();

        // Destructor
        ~ShaderStore();

        // Disable copy construction
        ShaderStore(const ShaderStore& other) = delete;
        ShaderStore& operator=(const ShaderStore& other) = delete;

        // Enable move construction
        ShaderStore(ShaderStore&& other) = default;
        ShaderStore& operator=(ShaderStore&& other) = default;

        // Compiles given shader from source
        GLuint LoadShader(const std::string& shaderSrc, ShaderType type);

        // Links the given shaders in a program
        bool LinkProgram(const std::string& name, GLuint vertShId, GLuint fragShId);

        // Retrieves the program id with the given name
        GLuint operator[](const std::string& name);

        // Unloads the loaded shaders and programs from the store
        void Clear();

        // Retrieves the last linking error if set
        const std::string& GetLastLinkError() const;

        // Retrieves the last compilation error if set
        const std::string& GetLastCompileError() const;

    private:
        // Stored shaders
        std::vector<GLuint> mShaders;

        // Stored programs
        std::unordered_map<std::string, GLuint> mPrograms;

        // Error state
        std::string mLastLinkError;
        std::string mLastCompileError;
};

#endif // ! _SHADERSTORE_HPP_
