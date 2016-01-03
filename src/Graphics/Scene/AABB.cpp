#include "AABB.hpp"

AABB::AABB()
{
}

AABB::AABB(glm::vec3 minPoint, glm::vec3 maxPoint)
    : minPoint(minPoint),
      maxPoint(maxPoint)
{
}

float AABB::MinX() const { return minPoint.x; }
float AABB::MaxX() const { return maxPoint.x; }
float AABB::MinY() const { return minPoint.y; }
float AABB::MaxY() const { return maxPoint.y; }
float AABB::MinZ() const { return minPoint.z; }
float AABB::MaxZ() const { return maxPoint.z; }
