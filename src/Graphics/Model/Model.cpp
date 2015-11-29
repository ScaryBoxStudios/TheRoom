#include "Model.hpp"

Model::Model()
{
    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mColBuf);
    glGenBuffers(1, &mTexBuf);
    glGenBuffers(1, &mEbo);
}

Model::~Model()
{
    glDeleteBuffers(1, &mEbo);
    glDeleteBuffers(1, &mTexBuf);
    glDeleteBuffers(1, &mColBuf);
    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);
}

Model::Model(Model&& other) :
    mVao(other.mVao),
    mVbo(other.mVbo),
    mColBuf(other.mColBuf),
    mTexBuf(other.mTexBuf),
    mEbo(other.mEbo)
{
    other.mVao = 0;
    other.mVbo = 0;
    other.mColBuf = 0;
    other.mTexBuf = 0;
    other.mEbo = 0;
}

Model& Model::operator=(Model&& other)
{
    if (this != &other)
    {
        mVao = other.mVao;
        mVbo = other.mVbo;
        mColBuf = other.mColBuf;
        mTexBuf = other.mTexBuf;
        mEbo = other.mEbo;

        other.mVao = 0;
        other.mVbo = 0;
        other.mColBuf = 0;
        other.mTexBuf = 0;
        other.mEbo = 0;
    }
    return *this;
}

void Model::Load(const ModelData& data)
{
    glBindVertexArray(mVao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        {
            glBufferData(GL_ARRAY_BUFFER,
                data.vertices.size() * sizeof(GLfloat),
                data.vertices.data(),
                GL_STATIC_DRAW
            );
            GLuint index = 0;
            glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(index);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, mColBuf);
        {
            glBufferData(GL_ARRAY_BUFFER,
                data.colors.size() * sizeof(GLfloat),
                data.colors.data(),
                GL_STATIC_DRAW
            );
            GLuint index = 1;
            glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(index);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, mTexBuf);
        {
            glBufferData(GL_ARRAY_BUFFER,
                data.texCoords.size() * sizeof(GLfloat),
                data.texCoords.data(),
                GL_STATIC_DRAW
            );
            GLuint index = 2;
            glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(index);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                data.indices.size() * sizeof(GLuint),
                data.indices.data(),
                GL_STATIC_DRAW
            );
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
}

GLuint Model::VaoId() const
{
    return mVao;
}

GLuint Model::EboId() const
{
    return mEbo;
}
