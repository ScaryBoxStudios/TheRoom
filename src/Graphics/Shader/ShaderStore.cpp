#include "ShaderStore.hpp"

ShaderStore::ShaderStore()
{
}

ShaderStore::~ShaderStore()
{
    Clear();
}

void ShaderStore::Clear()
{
    for (auto& shaderId: mShaders)
        glDeleteShader(shaderId);
    mShaders.clear();

    for (auto& p : mPrograms)
        glDeleteProgram(p.second);
    mPrograms.clear();
}

GLuint ShaderStore::LoadShader(const std::string& shaderSrc, ShaderType type)
{
    // Get raw str ptr
    const GLchar* s = shaderSrc.c_str();

    // Create shader, attach source and compile it
    GLuint shaderId = glCreateShader(static_cast<GLenum>(type));
    glShaderSource(shaderId, 1, &s, 0);
    glCompileShader(shaderId);

    // Check compilation result
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            std::vector<GLchar> buf(logLength, 0);
            glGetShaderInfoLog(shaderId, logLength, 0, buf.data());
            mLastCompileError = std::string(std::begin(buf), std::end(buf));
        }
        glDeleteShader(shaderId);
        return 0;
    }

    // Store and return successfully compiled shader
    mShaders.push_back(shaderId);
    return shaderId;
}

bool ShaderStore::LinkProgram(const std::string& name, GLuint vertShId, GLuint fragShId)
{
    // Create program, attach shaders and link
    GLuint progId = glCreateProgram();
    glAttachShader(progId, vertShId);
    glAttachShader(progId, fragShId);
    glLinkProgram(progId);

    // Check link result
    GLint linkStatus;
    glGetProgramiv(progId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(progId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0)
        {
            std::vector<GLchar> buf(logLength, 0);
            glGetShaderInfoLog(progId, logLength, 0, buf.data());
            mLastLinkError = std::string(std::begin(buf), std::end(buf));
        }
        glDeleteProgram(progId);
        return false;
    }

    // Store and return successfully linked program
    mPrograms.insert({name, progId});
    return true;
}

GLuint ShaderStore::operator[](const std::string& name)
{
    auto it = mPrograms.find(name);
    if (it == std::end(mPrograms))
        return 0;
    else
        return it->second;
}

const std::string& ShaderStore::GetLastCompileError() const
{
    return mLastCompileError;
}

const std::string& ShaderStore::GetLastLinkError() const
{
    return mLastLinkError;
}

