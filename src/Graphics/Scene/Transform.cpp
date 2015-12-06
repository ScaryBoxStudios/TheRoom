#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() :
    mParent(nullptr),
    mTransform(),
    mPosition(0.0f),
    mRotation(),
    mScale(1.0f),
    mShouldRecalculate(false)
{
}

const glm::mat4& Transform::Get()
{
    if (mShouldRecalculate)
    {
        CalcTransform();
        mShouldRecalculate = false;
    }

    return mTransform;
}

void Transform::CalcTransform()
{
    mTransform = glm::scale(mTransform, mScale);
    mTransform = mTransform * mRotation;
    mTransform = glm::translate(mTransform, mPosition);
    if (mParent != nullptr)
        mTransform = mTransform * mParent->Get();
}

const glm::vec3& Transform::GetPosition() const
{
    return mPosition;
}

void Transform::Move(const glm::vec3& pos)
{
    mPosition += pos;
    mShouldRecalculate = true;
}

void Transform::SetPos(const glm::vec3& pos)
{
    mPosition = pos;
    mShouldRecalculate = true;
}

void Transform::Rotate(float angle, const glm::vec3& normal)
{
    mTransform = glm::rotate(mTransform, angle, normal);
}

void Transform::Scale(const glm::vec3& scale)
{
    mScale += scale;
    mShouldRecalculate = true;
}

void Transform::SetScale(const glm::vec3& scale)
{
    mScale = scale;
    mShouldRecalculate = true;
}

void Transform::Invalidate()
{
    mShouldRecalculate = true;
}

void Transform::SetParent(Transform* par)
{
    mParent = par;
}

