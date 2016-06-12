#include "Engine.hpp"
#include "../Window/GlfwError.hpp"
#include "../Util/FileLoad.hpp"
#include "../Graphics/Shader/Shader.hpp"
#include "../Graphics/Shader/ShaderPreprocessor.hpp"
#include "../Util/Hash.hpp"
#include "../Util/MsgBox.hpp"

WARN_GUARD_ON
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

static std::unordered_map<std::string, ShaderProgram> LoadShaders()
{
    // Load the shader files
    std::vector<std::string> shaderFiles =
    {
        // Shaders
        "res/Shaders/geometry_pass_vert.glsl"
      , "res/Shaders/geometry_pass_frag.glsl"
      , "res/Shaders/light_pass_vert.glsl"
      , "res/Shaders/light_pass_frag.glsl"
        // Includes
      , "res/Shaders/include/lighting.glsl"
      , "res/Shaders/include/shadowing.glsl"
      , "res/Shaders/include/material.glsl"
      , "res/Shaders/include/brdf.glsl"
      , "res/Shaders/include/math.glsl"
    };

    std::unordered_map<std::string, std::string> loadedShaders;
    for (const auto& filepath : shaderFiles)
    {
        // Load file
        auto shaderFile = FileLoad<BufferType>(filepath);
        if (!shaderFile)
            throw std::runtime_error("Could not find shader file: \n" + filepath);

        // Convert it to std::string containter
        std::string shaderSrc((*shaderFile).begin(), (*shaderFile).end());

        // Store it to the loaded shader data vector
        loadedShaders.insert({filepath, shaderSrc});
    }

    // Preprocess them
    ShaderPreprocessor shaderPreprocessor;
    std::vector<std::string> deps;
    for (const auto& dep : loadedShaders)
        deps.push_back(dep.second);
    for (auto& shs : loadedShaders)
        shs.second = shaderPreprocessor.Preprocess(shs.second, deps);

    // The shader map
    std::unordered_map<std::string, std::vector<std::pair<Shader::Type, std::string>>> shadersLoc =
    {
        {
            "geometry_pass",
            {
                { Shader::Type::Vertex,   "res/Shaders/geometry_pass_vert.glsl" },
                { Shader::Type::Fragment, "res/Shaders/geometry_pass_frag.glsl" },
            }
        },
        {
            "light_pass",
            {
                { Shader::Type::Vertex,   "res/Shaders/light_pass_vert.glsl" },
                { Shader::Type::Fragment, "res/Shaders/light_pass_frag.glsl" },
            }
        }
    };

    // Load shaders
    std::unordered_map<std::string, ShaderProgram> shaderPrograms;
    for (const auto& p : shadersLoc)
    {
        // Will temporarily store the the compiled shaders
        std::unordered_map<Shader::Type, Shader> shaders;
        for (const auto& f : p.second)
        {
            // Get the source
            std::string shaderSrc = loadedShaders[f.second];

            // Create shader from source
            Shader shader(shaderSrc, f.first);

            // Insert loaded shader id to temp map
            shaders.emplace(f.first, std::move(shader));
        }

        // Fetch vert and frag shader id's from temp map and link
        ShaderProgram program(
            shaders.at(Shader::Type::Vertex).Id(),
            shaders.at(Shader::Type::Fragment).Id()
        );
        shaderPrograms.emplace(p.first, std::move(program));
    }
    return shaderPrograms;
}

void Engine::Init()
{
    // Setup window
    bool success = mWindow.Create(800, 600, "TheRoom", Window::Mode::Windowed);
    if (!success)
        throw std::runtime_error(GetLastGlfwError().GetDescription());

    mWindow.SetShowStats(true);
    mWindow.SetFramebufferResizeHandler(
        [this](int w, int h)
        {
            if (w == 0 && h == 0)
                return;
            glViewport(0, 0, w, h);
            mRenderer.Resize(w, h);
            mTextRenderer.Resize(w, h);
            mDbgRenderer.SetWindowDimensions(w, h);
            mDbgRenderer.SetDebugTextures(mRenderer.GetTextureTargets());
        }
    );

    // Setup console
    mConsole.Init();
    mConsoleIsActive = false;
    mWindow.AddKeyHook(
        [this](Key k, KeyAction ka) -> bool
        {
            if (k == Key::F10 && ka == KeyAction::Press)
                mConsoleIsActive = !mConsoleIsActive;
            if (mConsoleIsActive)
            {
                if (k == Key::Enter && ka == KeyAction::Press)
                {
                    mConsole.OnKey('\r');
                    mConsoleIsActive = false;
                }
                if (k == Key::Escape && ka == KeyAction::Release)
                {
                    mConsole.Cancel();
                    mConsoleIsActive = false;
                    return false; // Do not propagate this event
                }
                if (k == Key::Backspace && (ka == KeyAction::Press || ka == KeyAction::Repeat))
                    mConsole.OnKey('\b');
            }
            return !mConsoleIsActive;
        },
        Window::HookPos::Start
    );
    mWindow.AddCharHook(
        [this](char c) -> bool
        {
            if (mConsoleIsActive)
                mConsole.OnKey(c);
            return !mConsoleIsActive;
        },
        Window::HookPos::Start
    );
    mWindow.SetCharEnterHandler([](char){});

    // Load the needed shaders
    std::unordered_map<std::string, ShaderProgram> shdrProgs = LoadShaders();

    // Initialize the renderer
    mRenderer.Init(
        mWindow.GetWidth(),
        mWindow.GetHeight(),
        std::make_unique<Renderer::ShaderPrograms>(
            Renderer::ShaderPrograms
            {
                std::move(shdrProgs.at("geometry_pass")),
                std::move(shdrProgs.at("light_pass"))
            }
        )
    );

    // Pass the data store instances to renderer
    mRenderer.SetDataStores(&mMaterialStore);

    // Initialize the AABBRenderer
    mAABBRenderer.Init();
    mAABBRenderer.SetProjection(
        glm::perspective(
            45.0f,
            static_cast<float>(mWindow.GetWidth()) / mWindow.GetHeight(),
            0.1f, 300.0f
        )
    );

    // Initialize the TextRenderer
    mTextRenderer.Init(
        mWindow.GetWidth(),
        mWindow.GetHeight()
    );

    // Initialize the DebugRenderer
    mDbgRenderer.Init(
        mWindow.GetWidth(),
        mWindow.GetHeight()
    );
    mDbgRenderer.SetDebugTextures(mRenderer.GetTextureTargets());

    // Initialize the ConsoleRenderer
    mConsoleRenderer.Init(&mTextRenderer);

    // Initialize the SkyboxRenderer
    mSkyboxRenderer.Init();
}

void Engine::ReloadShaders()
{
    try
    {
        std::unordered_map<std::string, ShaderProgram> shdrProgs = LoadShaders();
        mRenderer.SetShaderPrograms(
            std::make_unique<Renderer::ShaderPrograms>(
                Renderer::ShaderPrograms
                {
                    std::move(shdrProgs.at("geometry_pass")),
                    std::move(shdrProgs.at("light_pass"))
                }
            )
        );
    }
    catch(const std::runtime_error& e)
    {
        MsgBox("Error", e.what()).Show();
    }
}

void Engine::Update(float dt)
{
    // Poll window events
    mWindow.Update();

    // Update the interpolation state of the world
    mRenderer.Update(dt);
}

void Engine::Render(float interpolation)
{
    (void) interpolation;
    // Render console
    if (mConsoleIsActive)
        mConsoleRenderer.Render(mConsole, mWindow.GetWidth(), mWindow.GetHeight());

    // Show rendered backbuffer
    mWindow.SwapBuffers();
}

void Engine::Shutdown()
{
    // Console
    mConsole.Shutdown();

    // ConsoleRenderer
    mConsoleRenderer.Shutdown();

    // DebugRenderer
    mDbgRenderer.Shutdown();

    // TextRenderer
    mTextRenderer.Shutdown();

    // AABBRenderer
    mAABBRenderer.Shutdown();

    // Renderer
    mRenderer.Shutdown();

    // Skybox Renderer
    mSkyboxRenderer.Shutdown();

    // Explicitly deallocate GPU texture data
    mTextureStore.Clear();

    // Explicitly deallocate GPU geometry data
    mModelStore.Clear();

    // Explicitly deallocate GPU cubemap data
    mCubemapStore.Clear();

    // Window
    mWindow.Destroy();
}

Window& Engine::GetWindow()
{
    return mWindow;
}

Console& Engine::GetConsole()
{
    return mConsole;
}

ModelStore& Engine::GetModelStore()
{
    return mModelStore;
}

TextureStore& Engine::GetTextureStore()
{
    return mTextureStore;
}

MaterialStore& Engine::GetMaterialStore()
{
    return mMaterialStore;
}

CubemapStore& Engine::GetCubemapStore()
{
    return mCubemapStore;
}

Renderer& Engine::GetRenderer()
{
    return mRenderer;
}

AABBRenderer& Engine::GetAABBRenderer()
{
    return mAABBRenderer;
}

TextRenderer& Engine::GetTextRenderer()
{
    return mTextRenderer;
}

DebugRenderer& Engine::GetDebugRenderer()
{
    return mDbgRenderer;
}

SkyboxRenderer& Engine::GetSkyboxRenderer()
{
    return mSkyboxRenderer;
}
