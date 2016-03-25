#include "GLUtils.hpp"
#include <vector>
#include <sstream>
#include <GL/glu.h>

void CheckGLError()
{
    GLenum errVal = glGetError();
    if (errVal != GL_NO_ERROR)
    {
        std::stringstream ss;
        ss << "OpenGL Error! Code: " << errVal;
        const char* desc = reinterpret_cast<const char*>(gluErrorString(errVal));
        throw std::runtime_error(std::string("OpenGL error: \n") + desc);
    }
}

std::string GetLastCompileError(GLuint shaderId)
{
    // Check compilation result
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        // Gather error string
        GLint logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            std::vector<GLchar> buf(logLength, 0);
            glGetShaderInfoLog(shaderId, logLength, 0, buf.data());
            return std::string(std::begin(buf), std::end(buf));
        }
    }
    return "";
}

std::string GetLastLinkError(GLuint progId)
{
    // Check link result
    GLint linkStatus;
    glGetProgramiv(progId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        // Gather error string
        GLint logLength;
        glGetProgramiv(progId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            std::vector<GLchar> buf(logLength, 0);
            glGetProgramInfoLog(progId, logLength, 0, buf.data());
            return std::string(std::begin(buf), std::end(buf));
        }
    }
    return "";
}
