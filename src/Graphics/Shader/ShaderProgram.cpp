#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram()
{
    mId = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(mId);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) :
    mId(other.mId),
    mLastLinkError(other.mLastLinkError)
{
    other.mId = 0;
    other.mLastLinkError = "";
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
{
    if (this != &other)
    {
        mId = other.mId;
        mLastLinkError = other.mLastLinkError;
        other.mId = 0;
        other.mLastLinkError = "";
    }
    return *this;
}

bool ShaderProgram::Link(const GLuint& vert, const GLuint& frag)
{
    glAttachShader(mId, vert);
    glAttachShader(mId, frag);

    glLinkProgram(mId);

    GLint linkStatus;
    glGetProgramiv(mId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            std::vector<GLchar> buf(logLength, 0);
            glGetShaderInfoLog(mId, logLength, 0, buf.data());
            mLastLinkError = std::string(std::begin(buf), std::end(buf));
            return false;
        }
    }
    return true;
}

GLuint ShaderProgram::Id() const
{
    return mId;
}

const std::string& ShaderProgram::GetLastLinkError() const
{
    return mLastLinkError;
}

