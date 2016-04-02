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
#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include <string>
#include <glad/glad.h>

class Shader
{
    public:
        // The shader type being created
        enum class Type
        {
            Vertex = GL_VERTEX_SHADER,
            Geometry = GL_GEOMETRY_SHADER,
            Fragment = GL_FRAGMENT_SHADER
        };

        // Constructor, throws std::runtime_error if source fails to compile
        Shader(const std::string& source, Type type);

        // Destructor
        ~Shader();

        // Disable copy construction
        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;

        // Enable move construction
        Shader(Shader&& other);
        Shader& operator=(Shader&& other);

        // Retrieves internal id
        GLuint Id() const;

    private:
        GLuint mId;
};

class ShaderProgram
{
    public:
        // Constructor, throws std::runtime_error if program fails to link
        ShaderProgram(GLuint vertShId, GLuint fragShId);
        ShaderProgram(GLuint vertShId, GLuint geomShId, GLuint fragShId);

        // Destructor
        ~ShaderProgram();

        // Disable copy construction
        ShaderProgram(const ShaderProgram& other) = delete;
        ShaderProgram& operator=(const ShaderProgram& other) = delete;

        // Enable move construction
        ShaderProgram(ShaderProgram&& other);
        ShaderProgram& operator=(ShaderProgram&& other);

        // Retrieves internal id
        GLuint Id() const;

    private:
        GLuint mId;
};

#endif // ! _SHADER_HPP_
