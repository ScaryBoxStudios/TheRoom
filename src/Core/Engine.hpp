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

#include "Console.hpp"
#include "../Window/Window.hpp"
#include "../Graphics/Resource/CubemapStore.hpp"
#include "../Graphics/Resource/TextureStore.hpp"
#include "../Graphics/Resource/ModelStore.hpp"
#include "../Graphics/Resource/MaterialStore.hpp"
#include "../Graphics/Renderer/Renderer.hpp"
#include "../Graphics/Renderer/AABBRenderer.hpp"
#include "../Graphics/Renderer/TextRenderer.hpp"
#include "../Graphics/Renderer/DebugRenderer.hpp"
#include "../Graphics/Renderer/ConsoleRenderer.hpp"
#include "../Graphics/Renderer/SkyboxRenderer.hpp"

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
        // Retrieves the console instance
        Console& GetConsole();

        // Retrieves the ModelStore instance
        ModelStore& GetModelStore();
        // Retrieves the TextureStore instance
        TextureStore& GetTextureStore();
        // Retrieves the MaterialStore instance
        MaterialStore& GetMaterialStore();
        // Retrieves the CubemapStore instance
        CubemapStore& GetCubemapStore();

        // Retrieves the renderer instance
        Renderer& GetRenderer();
        // Retrieves the AABB renderer instance
        AABBRenderer& GetAABBRenderer();
        // Retrieves the text renderer instance
        TextRenderer& GetTextRenderer();
        // Retrieves the debug renderer instance
        DebugRenderer& GetDebugRenderer();
        // Retrieves the skybox renderer instance
        SkyboxRenderer& GetSkyboxRenderer();

        // Tries to reload the Renderer's shaders from disk
        void ReloadShaders();

    private:
        // The interactive console instance
        Console mConsole;
        bool mConsoleIsActive;

        // The Game Window
        Window mWindow;

        // Stores the geometry data loaded in the gpu
        ModelStore mModelStore;
        // Stores the textures loaded in the gpu
        TextureStore mTextureStore;
        // Stores the loaded materials
        MaterialStore mMaterialStore;
        // Stores the loaded cubemaps
        CubemapStore mCubemapStore;

        // The Renderer
        Renderer mRenderer;
        // The AABB rendering utility
        AABBRenderer mAABBRenderer;
        // The Text Renderer
        TextRenderer mTextRenderer;
        // The Debug Renderer
        DebugRenderer mDbgRenderer;
        // The Console Renderer
        ConsoleRenderer mConsoleRenderer;
        // The Skybox Renderer
        SkyboxRenderer mSkyboxRenderer;
};

#endif // ! _ENGINE_HPP_
