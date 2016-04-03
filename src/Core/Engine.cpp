#include "Engine.hpp"
#include "../Window/GlfwError.hpp"
#include "../Util/FileLoad.hpp"
#include "../Graphics/Shader/ShaderPreprocessor.hpp"
#include "../Util/Hash.hpp"

WARN_GUARD_ON
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

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
            glViewport(0, 0, w, h);
            mRenderer.Resize(w, h);
            mDbgRenderer.SetWindowDimensions(w, h);
            mDbgRenderer.SetDebugTextures(mRenderer.GetTextureTargets());
        }
    );

    // Load the needed shaders
    LoadShaders();

    // Initialize the renderer
    mRenderer.Init(
        mWindow.GetWidth(),
        mWindow.GetHeight(),
        mShaderPrograms.at("geometry_pass").Id(),
        mShaderPrograms.at("light_pass").Id()
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
    // Show rendered backbuffer
    mWindow.SwapBuffers();
}

void Engine::Shutdown()
{
    // Remove cached shader programs
    mShaderPrograms.clear();

    // DebugRenderer
    mDbgRenderer.Shutdown();

    // TextRenderer
    mTextRenderer.Shutdown();

    // AABBRenderer
    mAABBRenderer.Shutdown();

    // Renderer
    mRenderer.Shutdown();

    // Explicitly deallocate GPU texture data
    mTextureStore.Clear();

    // Explicitly deallocate GPU geometry data
    mModelStore.Clear();

    // Window
    mWindow.Destroy();
}

Window& Engine::GetWindow()
{
    return mWindow;
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

void Engine::AddShaderProgram(const std::string& name, ShaderProgram sp)
{
    mShaderPrograms.emplace(name, std::move(sp));
}

void Engine::LoadShaders()
{
    // Load the shader files
    std::vector<std::string> shaderFiles =
    {
        "res/geometry_pass_vert.glsl"
      , "res/geometry_pass_frag.glsl"
      , "res/light_pass_vert.glsl"
      , "res/light_pass_frag.glsl"
      , "res/lighting.glsl"
      , "res/shadowing.glsl"
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
                { Shader::Type::Vertex,   "res/geometry_pass_vert.glsl" },
                { Shader::Type::Fragment, "res/geometry_pass_frag.glsl" },
            }
        },
        {
            "light_pass",
            {
                { Shader::Type::Vertex,   "res/light_pass_vert.glsl" },
                { Shader::Type::Fragment, "res/light_pass_frag.glsl" },
            }
        }
    };

    // Load shaders
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
        AddShaderProgram(p.first, std::move(program));
    }
}
