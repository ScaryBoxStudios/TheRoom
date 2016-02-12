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
#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include "../Window/Window.hpp"
#include "../Graphics/Texture/TextureStore.hpp"
#include "../Graphics/Renderer/Renderer.hpp"
#include "../Graphics/Renderer/Skybox.hpp"
#include "../Graphics/Shader/Shader.hpp"

class Engine
{
    public:
        /*! Initializes all the low level modules of the game */
        void Init();

        /*! Called when updating the game state */
        void Update(float dt);

        /*! Called when rendering the current frame */
        void Render(float interpolation);

        /*! Deinitializes all the low level modules of the game */
        void Shutdown();

        // Retrieves the window instance
        Window& GetWindow();

        // Retrieves the TextureStore instance
        TextureStore& GetTextureStore();

        // Retrieves the renderer instance
        Renderer& GetRenderer();

        // Retrieves the text renderer instance
        TextRenderer& GetTextRenderer();

        // Sets adds a ShaderProgram to the program list
        void AddShaderProgram(const std::string& name, ShaderProgram sp);

        // Sets the skybox that is used by the renderer
        void SetSkybox(std::unique_ptr<Skybox> skybox);

    private:
        // Loads the default shaders used
        void LoadShaders();

        // The Game Window
        Window mWindow;

        // Stores the textures loaded in the gpu
        TextureStore mTextureStore;

        // The Renderer
        Renderer mRenderer;

        // The Text Renderer
        TextRenderer mTextRenderer;

        // The loaded ShaderProgram(s)
        std::unordered_map<std::string, ShaderProgram> mShaderPrograms;

        // The skybox used
        std::unique_ptr<Skybox> mSkybox;
};

#endif // ! _ENGINE_HPP_
