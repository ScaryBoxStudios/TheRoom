#include "Shader.hpp"

Shader::Shader(Type type)
{
    mId = glCreateShader(static_cast<GLuint>(type));
}

Shader::~Shader()
{
    glDeleteShader(mId);
}

Shader::Shader(Shader&& other) :
    mId(other.mId),
    mLastCompileError(other.mLastCompileError)
{
    other.mId = 0;
    other.mLastCompileError = "";
}

Shader& Shader::operator=(Shader&& other)
{
    if (this != &other)
    {
        mId = other.mId;
        mLastCompileError = other.mLastCompileError;
        other.mId = 0;
        other.mLastCompileError = "";
    }
    return *this;
}

bool Shader::Source(const std::string& src)
{
    const GLchar* s = src.c_str();
    glShaderSource(mId, 1, &s, 0);

    glCompileShader(mId);

    GLint compileStatus;
    glGetShaderiv(mId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            std::vector<GLchar> buf(logLength, 0);
            glGetShaderInfoLog(mId, logLength, 0, buf.data());
            mLastCompileError = std::string(std::begin(buf), std::end(buf));
            return false;
        }
    }
    return true;
}

GLuint Shader::Id() const
{
    return mId;
}

const std::string& Shader::GetLastCompileError() const
{
    return mLastCompileError;
}
