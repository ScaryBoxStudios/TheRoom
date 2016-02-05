#include "Screen.hpp"

ScreenContext::ScreenContext(Engine* e)
  : mEngine(e)
{
}

Engine* ScreenContext::GetEngine()
{
    return mEngine;
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
