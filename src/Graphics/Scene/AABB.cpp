#include "AABB.hpp"

WARN_GUARD_ON
#include <glm/gtc/matrix_transform.hpp>
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

void AABB::Move(const glm::vec3& offset)
{
    transform.Move(offset);
}

void AABB::Scale(const glm::vec3& factor)
{
    transform.Scale(factor);
}

void AABB::RotateX(float angle)
{
}

void AABB::RotateY(float angle)
{
}

void AABB::RotateZ(float angle)
{
}

void AABB::Update()
{
    cachedMin = glm::vec3(transform.Get() * glm::vec4(localMin, 1));
    cachedMax = glm::vec3(transform.Get() * glm::vec4(localMax, 1));
}
