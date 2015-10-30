#include "GlfwContext.hpp"
#include <string>
#include <GLFW/glfw3.h>

///==============================================================
///= GlfwContext
///==============================================================

bool GlfwContext::sInitialized = false;

bool GlfwContext::Init()
{
    RegisterGlfwErrorHandler();
    glfwInit();

    if (IsGlfwErrorSet())
        return false;

    sInitialized = true;
    return true;
}

void GlfwContext::Shutdown()
{
    glfwTerminate();
    sInitialized = false;
}

bool GlfwContext::IsInitialized()
{
    return sInitialized;
}


