#include "GlfwError.hpp"
#include <GLFW/glfw3.h>

///==============================================================
///= GlfwError
///==============================================================
GlfwError::GlfwError(int errorCode, std::string desc)
    : mErrCode(errorCode),
      mDescription(desc)
{
}
int GlfwError::GetErrorCode() const { return mErrCode; }
const std::string& GlfwError::GetDescription() const { return mDescription; }

///==============================================================
///= Error API
///==============================================================
static bool errorFlag = false;
static GlfwError lastError(0, "");
static void glfwErrCb(int code, const char* desc)
{
    lastError = GlfwError(code, desc);
    errorFlag = true;
}

void RegisterGlfwErrorHandler()
{
    glfwSetErrorCallback(glfwErrCb);
}

bool IsGlfwErrorSet() { return errorFlag; }

GlfwError GetLastGlfwError()
{
    // Store prev error
    GlfwError curErr = lastError;
    // Reset
    lastError = GlfwError(0, "");
    errorFlag = false;
    // Return copy
    return curErr;
}

