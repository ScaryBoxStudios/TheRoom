#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() :
    mParent(nullptr),
    mTransform(),
    mPosition(0.0f),
    mYaw(0.0f),
    mPitch(0.0f),
    mRoll(0.0f),
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
    mTransform = glm::translate(mTransform, mPosition);
    mTransform = glm::rotate(mTransform, mYaw, glm::vec3(0, 0, 1));
    mTransform = glm::rotate(mTransform, mPitch, glm::vec3(0, 1, 0));
    mTransform = glm::rotate(mTransform, mRoll, glm::vec3(1, 0, 0));
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

void Transform::RotateX(float angle)
{
    mRoll += angle;
    mShouldRecalculate = true;
}

void Transform::RotateY(float angle)
{
    mPitch += angle;
    mShouldRecalculate = true;
}

void Transform::RotateZ(float angle)
{
    mYaw += angle;
    mShouldRecalculate = true;
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

