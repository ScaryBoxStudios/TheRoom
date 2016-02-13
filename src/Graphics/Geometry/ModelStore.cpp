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
            glDeleteBuffers(1, &meshDesc.eboId);
            glDeleteBuffers(1, &meshDesc.vboId);
            glDeleteVertexArrays(1, &meshDesc.vaoId);
        }
        modelDesc.meshes.clear();
    }
    mModels.clear();
}

void ModelStore::Load(const std::string& name, const ModelData& data)
{
    ModelDescription modelDesc = {};

    for (const auto& mesh : data.meshes)
    {
        MeshDescription meshDesc;
        auto& vaoId = meshDesc.vaoId;
        auto& vboId = meshDesc.vboId;
        auto& eboId = meshDesc.eboId;
        auto& numIndices = meshDesc.numIndices;

        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &vboId);
        glGenBuffers(1, &eboId);

        glBindVertexArray(vaoId);
        {
            glBindBuffer(GL_ARRAY_BUFFER, vboId);
            {
                glBufferData(GL_ARRAY_BUFFER,
                    mesh.data.size() * sizeof(VertexData),
                    mesh.data.data(),
                    GL_STATIC_DRAW
                );

                // Vertices
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)(offsetof(VertexData, vx)));
                glEnableVertexAttribArray(0);

                // Normals
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)(offsetof(VertexData, nx)));
                glEnableVertexAttribArray(1);

                // TexCoords
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)(offsetof(VertexData, tx)));
                glEnableVertexAttribArray(2);

                // Tangent
                glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)(offsetof(VertexData, tnx)));
                glEnableVertexAttribArray(3);
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

    modelDesc.localAABB = data.boundingBox;
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

