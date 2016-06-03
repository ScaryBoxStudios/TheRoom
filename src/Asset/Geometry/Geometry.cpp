#include "Geometry.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
WARN_GUARD_OFF

ModelData GenUVSphere(float radius, std::uint32_t rings = 32, std::uint32_t sectors = 32)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<std::uint32_t> indices;

    // Alias
    const auto pi = glm::pi<float>();

    // Preallocate space
    vertices.resize(rings * sectors);
    normals.resize(rings * sectors);
    texCoords.resize(rings * sectors);
    indices.resize(rings * sectors * 6);

    //
    const float R = 1.0f / static_cast<float>(rings - 1);
    const float S = 1.0f / static_cast<float>(sectors - 1);

    // Calc the vertices
    auto v = std::begin(vertices);
    auto n = std::begin(normals);
    auto t = std::begin(texCoords);
    for (std::uint32_t r = 0; r < rings; ++r)
    {
        for (std::uint32_t s = 0; s < sectors; ++s)
        {
            float x = cosf(2 * pi * s * S) * sinf(pi * r * R);
            float z = sinf(2 * pi * s * S) * sinf(pi * r * R);
            float y = sinf(-pi / 2 + pi * r * R);

            // Push back vertex data
            *v++ = (glm::vec3(x, y, z) * radius);

            // Push back normal
            *n++ = (glm::vec3(x, y, z));

            // Push back texture coordinates
            *t++ = (glm::vec2(s * S, (rings - 1 - r) * R));
        }
    }

    // Calc indices
    auto it = std::begin(indices);
    for (std::uint32_t r = 0; r < rings - 1; ++r)
    {
        for (std::uint32_t s = 0; s < sectors - 1; ++s)
        {
            int curRow = r * sectors;
            int nextRow = (r + 1) * sectors;

            *it++ = (curRow + s);
            *it++ = (nextRow + s);
            *it++ = (nextRow + (s+1));

            *it++ = (curRow + s);
            *it++ = (nextRow + (s+1));
            *it++ = (curRow + (s+1));
        }
    }

    // Fill meshData struct
    MeshData meshData;
    for (std::size_t i = 0; i < vertices.size(); ++i)
    {
        meshData.data.emplace_back(VertexData {
            vertices[i].x, vertices[i].y, vertices[i].z,
            normals[i].x, normals[i].y, normals[i].z,
            texCoords[i].x, texCoords[i].y,
            0.0f, 0.0f, 0.0f
        });
    }
    meshData.indices = std::move(indices);

    // Add the mesh index
    meshData.meshIndex = 0;

    // Calculate the AABB
    AABB boundingBox(glm::vec3(-radius), glm::vec3(radius));

    ModelData modelData;
    modelData.meshes.push_back(std::move(meshData));
    modelData.boundingBox = boundingBox;
    return modelData;
}
