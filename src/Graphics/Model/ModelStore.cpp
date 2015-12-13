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
        auto& modelDesc = p.second;
        for (auto& meshDesc : modelDesc.meshes)
        {
            glDeleteBuffers(1, &meshDesc.normalBufId);
            glDeleteBuffers(1, &meshDesc.eboId);
            glDeleteBuffers(1, &meshDesc.texBufId);
            glDeleteBuffers(1, &meshDesc.colBufId);
            glDeleteBuffers(1, &meshDesc.vboId);
            glDeleteVertexArrays(1, &meshDesc.vaoId);
        }
        modelDesc.meshes.clear();
    }
    mModels.clear();
}

void ModelStore::Load(const std::string& name, const Model& data)
{
    ModelDescription modelDesc = {};

    for (const auto& mesh : data.meshes)
    {
        MeshDescription meshDesc;
        auto& vaoId = meshDesc.vaoId;
        auto& vboId = meshDesc.vboId;
        auto& colBufId = meshDesc.colBufId;
        auto& texBufId = meshDesc.texBufId;
        auto& eboId = meshDesc.eboId;
        auto& normalBufId = meshDesc.normalBufId;
        auto& numIndices = meshDesc.numIndices;

        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &vboId);
        glGenBuffers(1, &colBufId);
        glGenBuffers(1, &texBufId);
        glGenBuffers(1, &eboId);
        glGenBuffers(1, &normalBufId);

        glBindVertexArray(vaoId);
        {
            glBindBuffer(GL_ARRAY_BUFFER, vboId);
            {
                glBufferData(GL_ARRAY_BUFFER,
                    mesh.vertices.size() * sizeof(GLfloat),
                    mesh.vertices.data(),
                    GL_STATIC_DRAW
                );
                GLuint index = 0;
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
                glEnableVertexAttribArray(index);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, normalBufId);
            {
                glBufferData(GL_ARRAY_BUFFER,
                    mesh.normals.size() * sizeof(GLfloat),
                    mesh.normals.data(),
                    GL_STATIC_DRAW
                );
                GLuint index = 1;
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
                glEnableVertexAttribArray(index);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, colBufId);
            {
                glBufferData(GL_ARRAY_BUFFER,
                    mesh.colors.size() * sizeof(GLfloat),
                    mesh.colors.data(),
                    GL_STATIC_DRAW
                );
                GLuint index = 2;
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
                glEnableVertexAttribArray(index);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, texBufId);
            {
                glBufferData(GL_ARRAY_BUFFER,
                    mesh.texCoords.size() * sizeof(GLfloat),
                    mesh.texCoords.data(),
                    GL_STATIC_DRAW
                );
                GLuint index = 3;
                glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
                glEnableVertexAttribArray(index);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    mesh.indices.size() * sizeof(GLuint),
                    mesh.indices.data(),
                    GL_STATIC_DRAW
                );
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);
        numIndices = static_cast<GLsizei>(mesh.indices.size());

        modelDesc.meshes.push_back(meshDesc);
    }

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

