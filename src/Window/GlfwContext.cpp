#include "GlfwContext.hpp"
#include <string>
#include <stdexcept>
#include <GLFW/glfw3.h>

///==============================================================
///= GlfwContext
///==============================================================

bool GlfwContext::sInitialized = false;

void GlfwContext::Init()
{
    RegisterGlfwErrorHandler();
    glfwInit();

    if (IsGlfwErrorSet())
        throw std::runtime_error(GetLastGlfwError().GetDescription());

    sInitialized = true;
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

