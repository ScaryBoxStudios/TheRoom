#include "Shader.hpp"
#include <stdexcept>
#include "../Util/GLUtils.hpp"

Shader::Shader(const std::string& source, Type type)
{
    // Get raw str ptr
    const GLchar* s = source.c_str();

    // Create shader, attach source and compile it
    mId = glCreateShader(static_cast<GLenum>(type));
    glShaderSource(mId, 1, &s, 0);
    glCompileShader(mId);

    // Check compilation result
    std::string err = GetLastCompileError(mId);
    if (err != "")
    {
        glDeleteShader(mId);
        throw std::runtime_error(err);
    }
}

Shader::Shader(Shader&& other)
{
    this->mId = other.mId;
    other.mId = 0;
}

Shader& Shader::operator=(Shader&& other)
{
    this->mId = other.mId;
    other.mId = 0;
    return *this;
}

Shader::~Shader()
{
    if (mId)
        glDeleteShader(mId);
}

GLuint Shader::Id() const
{
    return mId;
}

ShaderProgram::ShaderProgram(GLuint vertShId, GLuint fragShId)
{
    // Create program, attach shaders and link
    mId = glCreateProgram();
    glAttachShader(mId, vertShId);
    glAttachShader(mId, fragShId);
    glLinkProgram(mId);

    // Check link result
    std::string err = GetLastLinkError(mId);
    if (err != "")
    {
        glDeleteProgram(mId);
        throw std::runtime_error(err);
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
{
    this->mId = other.mId;
    other.mId = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
{
    this->mId = other.mId;
    other.mId = 0;
    return *this;
}

ShaderProgram::~ShaderProgram()
{
    if (mId)
        glDeleteProgram(mId);
}

GLuint ShaderProgram::Id() const
{
    return mId;
}
