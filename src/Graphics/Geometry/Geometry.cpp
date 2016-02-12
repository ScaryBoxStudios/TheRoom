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

    //
    const float R = 1.0f / static_cast<float>(rings - 1);
    const float S = 1.0f / static_cast<float>(sectors - 1);

    // Calc the vertices
    for (std::uint32_t r = 0; r < rings; ++r)
    {
        for (std::uint32_t s = 0; s < sectors; ++s)
        {
            float x = cosf(2 * pi * s * S) * sinf(pi * r * R);
            float z = sinf(2 * pi * s * S) * sinf(pi * r * R);
            float y = sinf(-pi / 2 + pi * r * R);

            // Push back vertex data
            vertices.push_back(glm::vec3(x, y, z) * radius);

            // Push back indices
            {
                int curRow = r * sectors;
                int nextRow = (r + 1) * sectors;

                indices.push_back(curRow + s);
                indices.push_back(nextRow + s);
                indices.push_back(nextRow + (s+1));

                indices.push_back(curRow + s);
                indices.push_back(nextRow + (s+1));
                indices.push_back(curRow + (s+1));
            }

            // Push back normal
            normals.push_back(glm::vec3(x, y, z));

            // Push back texture coordinates
            texCoords.push_back(glm::vec2(s*S, r*R));
        }
    }

    // Fill meshData struct
    MeshData meshData;
    for (std::size_t i = 0; i < vertices.size(); ++i)
    {
        meshData.data.emplace_back(VertexData {
            vertices[i].x, vertices[i].y, vertices[i].z,
            normals[i].x, normals[i].y, normals[i].z,
            texCoords[i].x, texCoords[i].y
        });
    }
    meshData.indices = std::move(indices);

    // Calculate the AABB
    AABB boundingBox(glm::vec3(-radius), glm::vec3(radius));

    ModelData modelData;
    modelData.meshes.push_back(std::move(meshData));
    modelData.boundingBox = boundingBox;
    return modelData;
}
