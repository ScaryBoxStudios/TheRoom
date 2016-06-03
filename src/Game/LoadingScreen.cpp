#include "LoadingScreen.hpp"
#include <thread>
WARN_GUARD_ON
#include "../Asset/Image/ImageLoader.hpp"
#include "../Asset/Geometry/ModelLoader.hpp"
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"

void LoadingScreen::onInit(ScreenContext& sc)
{
    // Store the engine reference
    mEngine = sc.GetEngine();

    // Store the file data cache reference
    mFileDataCache = sc.GetFileDataCache();

    // Load font
    mEngine->GetTextRenderer().GetFontStore().LoadFont("visitor", "ext/Assets/Fonts/visitor.ttf");

    // Fire loader thread
    mFileCacheIsReady = false;
    std::thread loaderThread(
        [this]()
        {
            LoadFileData();
            mFileCacheIsReady = true;
        }
    );
    loaderThread.detach();
}

void LoadingScreen::LoadFileData()
{
    for (const auto& file : mFileList)
    {
        mCurrentlyLoading = file;
        mFileDataCache->emplace(file, FileLoad<BufferType>(file));
        if(!(*mFileDataCache)[file])
            throw std::runtime_error("Couldn't load file (" + file + ")");
    }
};

void LoadingScreen::onUpdate(float dt)
{
    mEngine->Update(dt);

    if (mFileCacheIsReady)
    {
        mOnLoadedCb();
        mFileCacheIsReady = false;
    }
}

void LoadingScreen::onRender(float interpolation)
{
    (void) interpolation;
    // Clear
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Show indicator
    std::string indicator = "Loading ";
    indicator += mCurrentlyLoading;
    indicator += "...";
    mEngine->GetTextRenderer().RenderText(indicator, 10, 10, glm::vec3(1.0f, 0.5f, 0.3f), "visitor");
}

void LoadingScreen::onShutdown()
{
}

void LoadingScreen::SetFileList(const std::vector<std::string>& fileList)
{
    mFileList = fileList;
}

void LoadingScreen::SetOnLoadedCb(OnLoadedCb cb)
{
    mOnLoadedCb = cb;
}
