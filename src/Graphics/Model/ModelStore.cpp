#include "ModelStore.hpp"

ModelStore::ModelStore()
{
}

ModelStore::~ModelStore()
{
    Clear();
}

void ModelStore::Clear()
{
    for (auto& p : mModels)
    {
        const auto& modelDesc = p.second;
        glDeleteBuffers(1, &modelDesc.eboId);
        glDeleteBuffers(1, &modelDesc.texBufId);
        glDeleteBuffers(1, &modelDesc.colBufId);
        glDeleteBuffers(1, &modelDesc.vboId);
        glDeleteVertexArrays(1, &modelDesc.vaoId);
    }
    mModels.clear();
}

void ModelStore::Load(const std::string& name, const ModelData& data)
{
    ModelDescription modelDesc;
    auto& vaoId = modelDesc.vaoId;
    auto& vboId = modelDesc.vboId;
    auto& colBufId = modelDesc.colBufId;
    auto& texBufId = modelDesc.texBufId;
    auto& eboId = modelDesc.eboId;

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glGenBuffers(1, &colBufId);
    glGenBuffers(1, &texBufId);
    glGenBuffers(1, &eboId);

    glBindVertexArray(vaoId);
    {
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
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

        glBindBuffer(GL_ARRAY_BUFFER, colBufId);
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

        glBindBuffer(GL_ARRAY_BUFFER, texBufId);
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

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
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

    mModels.insert({name, modelDesc});
}

ModelDescription* ModelStore::operator[](const std::string& name)
{
    auto it = mModels.find(name);
    if(it == std::end(mModels))
        return nullptr;
    else
        return &(it->second);
}

