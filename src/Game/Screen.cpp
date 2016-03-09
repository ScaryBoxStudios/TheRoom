#include "Screen.hpp"

ScreenContext::ScreenContext(Engine* e, ScreenContext::FileDataCache* fdc)
  : mEngine(e)
  , mFileDataCache(fdc)
{
}

Engine* ScreenContext::GetEngine()
{
    return mEngine;
}

ScreenContext::FileDataCache* ScreenContext::GetFileDataCache()
{
    return mFileDataCache;
}

void Screen::onKey(Key k, KeyAction ka)
{
    (void) k;
    (void) ka;
}

void Screen::onMouse(MouseButton mb, ButtonAction ba)
{
    (void) mb;
    (void) ba;
}

void Screen::Finish()
{
    mFinishCb();
}

void Screen::SetFinishCb(FinishCb cb)
{
    mFinishCb = cb;
}
