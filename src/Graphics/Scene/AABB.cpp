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
glm::vec3 AABB::Center() const { return (MinPoint() + MaxPoint()) * 0.5f; }
glm::vec3 AABB::Size() const
{
    return glm::vec3(
        MaxX() - MinX(),
        MaxY() - MinY(),
        MaxZ() - MinZ()
    );
}

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

bool Intersects(const glm::vec3& p, const AABB& box)
{
    return p.x > box.MinX() && p.x < box.MaxX()
        && p.y > box.MinY() && p.y < box.MaxY()
        && p.z > box.MinZ() && p.x < box.MaxZ();
}

bool Intersects(const AABB& aabb1, const AABB& aabb2)
{
    bool xCollide = aabb1.MaxX() > aabb2.MinX()
                 && aabb1.MinX() < aabb2.MaxX();
    bool yCollide = aabb1.MaxY() > aabb2.MinY()
                 && aabb1.MinY() < aabb2.MaxY();
    bool zCollide = aabb1.MaxZ() > aabb2.MinZ()
                 && aabb1.MinZ() < aabb2.MaxZ();
    return xCollide && yCollide && zCollide;
    /*
    return aabb1.MaxX() > aabb2.MinX()
        && aabb1.MinX() < aabb2.MaxX()
        && aabb1.MaxY() > aabb2.MinY()
        && aabb1.MinY() < aabb2.MaxY()
        && aabb1.MaxZ() > aabb2.MinZ()
        && aabb1.MinZ() < aabb2.MaxZ();
    */
}

glm::vec3 CalcCollisionResponce(const AABB& aabb1, const AABB& aabb2)
{
    bool xCollide = aabb1.MaxX() > aabb2.MinX();
    bool yCollide = aabb1.MaxY() > aabb2.MinY();
    bool zCollide = aabb1.MaxZ() > aabb2.MinZ();

    glm::vec3 resolution = {};
    if (xCollide)
        resolution.x = ((aabb2.Center().x - aabb1.Center().x) / fabs(aabb1.Center().x - aabb2.Center().x))
            * (fabs(aabb1.Center().x - aabb2.Center().x) - ((aabb1.Size().x + aabb2.Size().x) * 0.5f));

    if (yCollide)
        resolution.y = ((aabb2.Center().y - aabb1.Center().y) / fabs(aabb1.Center().y - aabb2.Center().y))
            * (fabs(aabb1.Center().y - aabb2.Center().y) - ((aabb1.Size().y + aabb2.Size().y) * 0.5f));

    if (zCollide)
        resolution.z = ((aabb2.Center().z - aabb1.Center().z) / fabs(aabb1.Center().z - aabb2.Center().z))
            * (fabs(aabb1.Center().z - aabb2.Center().z) - ((aabb1.Size().z + aabb2.Size().z) * 0.5f));

    // Keep only the min resolution
    float minResolution = std::fminf(
        std::fminf(
            fabs(resolution.x),
            fabs(resolution.y)
        )
        , fabs(resolution.z)
    );

    if (fabs(resolution.x) != minResolution)
        resolution.x = 0.0f;
    if (fabs(resolution.y) != minResolution)
        resolution.y = 0.0f;
    if (fabs(resolution.z) != minResolution)
        resolution.z = 0.0f;

    return resolution;
}
