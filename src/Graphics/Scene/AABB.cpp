#include "AABB.hpp"

WARN_GUARD_ON
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
WARN_GUARD_OFF

AABB::AABB()
{
}

AABB::AABB(glm::vec3 minPoint, glm::vec3 maxPoint)
    : localMin(minPoint),
      cachedMin(minPoint),
      localMax(maxPoint),
      cachedMax(maxPoint)
{
}

float AABB::MinX() const { return MinPoint().x; }
float AABB::MaxX() const { return MaxPoint().x; }
float AABB::MinY() const { return MinPoint().y; }
float AABB::MaxY() const { return MaxPoint().y; }
float AABB::MinZ() const { return MinPoint().z; }
float AABB::MaxZ() const { return MaxPoint().z; }

glm::vec3 AABB::MinPoint() const { return cachedMin; }
glm::vec3 AABB::MaxPoint() const { return cachedMax; }

void AABB::Update(const glm::vec3& transl, const glm::vec3& scale, const glm::vec3& rotation)
{
    glm::vec3 aabbPoints[] = {
        {localMin.x, localMin.y, localMin.z},
        {localMin.x, localMin.y, localMax.z},
        {localMin.x, localMax.y, localMin.z},
        {localMin.x, localMax.y, localMax.z},
        {localMax.x, localMin.y, localMin.z},
        {localMax.x, localMin.y, localMax.z},
        {localMax.x, localMax.y, localMin.z},
        {localMax.x, localMax.y, localMax.z},
    };

    auto newMin = localMin;
    auto newMax = localMax;
    for (int i = 0; i < 8; ++i)
    {
        aabbPoints[i] = glm::rotateX(aabbPoints[i], rotation.x);
        aabbPoints[i] = glm::rotateY(aabbPoints[i], rotation.y);
        aabbPoints[i] = glm::rotateZ(aabbPoints[i], rotation.z);

        if (aabbPoints[i].x < newMin.x)
            newMin.x = aabbPoints[i].x;
        if (aabbPoints[i].x > newMax.x)
            newMax.x = aabbPoints[i].x;
        if (aabbPoints[i].y < newMin.y)
            newMin.y = aabbPoints[i].y;
        if (aabbPoints[i].y > newMax.y)
            newMax.y = aabbPoints[i].y;
        if (aabbPoints[i].z < newMin.z)
            newMin.z = aabbPoints[i].z;
        if (aabbPoints[i].z > newMax.z)
            newMax.z = aabbPoints[i].z;
    }

    cachedMin = newMin;
    cachedMax = newMax;

    glm::mat4 transform = glm::mat4();
    transform = glm::translate(transform, transl);
    transform = glm::scale(transform, scale);
    cachedMin = glm::vec3(transform * glm::vec4(cachedMin, 1.0f));
    cachedMax = glm::vec3(transform * glm::vec4(cachedMax, 1.0f));
}
