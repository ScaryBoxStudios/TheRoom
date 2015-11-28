#ifndef _GAME_HPP_
#define _GAME_HPP_

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../Util/WarnGuard.hpp"
#include "../Window/Window.hpp"
#include "../Graphics/Util/Camera.hpp"
#include "../Graphics/Model/Model.hpp"
#include "../Graphics/Shader/Shader.hpp"
#include "../Graphics/Shader/ShaderProgram.hpp"
#include "../Graphics/Texture/Texture.hpp"

WARN_GUARD_ON
#include <glm/glm.hpp>
WARN_GUARD_OFF

class Game
{
    public:
        /*! Constructor */
        Game();

        /*! Initializes all the low level modules of the game */
        void Init();

        /*! Called by the mainloop to update the game state */
        void Update(float dt);

        /*! Called by the mainloop to render the current frame */
        void Render(float interpolation);

        /*! Deinitializes all the low level modules of the game */
        void Shutdown();

        /*! Sets the master exit callback that when called should stop the main loop */
        void SetExitHandler(std::function<void()> f);

    private:
        // Disable copy construction
        Game(const Game& other) = delete;
        Game& operator=(const Game& other) = delete;

        // Master switch, called when game is exiting
        std::function<void()> mExitHandler;

        // The game window
        Window mWindow;

        // Initializes the OpenGL data
        void GLInit();

        // The OpenGL data
        struct GLData
        {
            GLenum drawMode;
        };
        GLData mGLData;

        // Stores the models loaded in the gpu
        std::unordered_map<std::string, std::unique_ptr<Model>> mModelStore;

        // Stores the shader programs loaded in the gpu
        std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> mShaderProgramStore;

        // Stores the textures loaded in the gpu
        std::unordered_map<std::string, std::unique_ptr<Texture>> mTextureStore;

        // GameObject
        struct GameObject
        {
            glm::vec3 position;
            Model* model;
        };
        std::vector<GameObject> mWorld;

        // The data needed for rendering
        struct RenderData
        {
            float degrees;
            float degreesInc;
        };
        RenderData mRenderData;

        // The camera view
        std::vector<Camera::MoveDirection> CameraMoveDirections();
        std::tuple<float, float> CameraLookOffset();
        Camera mCamera;
};

#endif // ! _GAME_HPP_

