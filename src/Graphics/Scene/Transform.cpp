#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() :
    mParent(nullptr),
    mTransform(glm::mat4()),
    mPosition(0.0f),
    mYaw(0.0f),
    mPitch(0.0f),
    mRoll(0.0f),
    mScale(1.0f),
    mOldTrans()
{
}

const glm::mat4& Transform::Get() const
{
    //if (mParent != nullptr) // TODO
    //    mTransform = mTransform * mParent->Get();
    return mTransform;
}

void Transform::Update()
{
    mOldTrans = mTransform;
}

glm::mat4 Transform::GetInterpolated(float interpolation) const
{
    glm::mat4 iTransform = mOldTrans + (mTransform - mOldTrans) * interpolation;
    return iTransform;
}

const glm::vec3& Transform::GetPosition() const
{
    return mPosition;
}

const glm::vec3& Transform::GetScale() const
{
    return mScale;
}

const glm::vec3 Transform::GetRotation() const
{
    return glm::vec3(mRoll, mPitch, mYaw);
}

void Transform::Move(const glm::vec3& pos)
{
    mPosition += pos * mScale;
    mTransform = glm::translate(mTransform, pos);
}

void Transform::RotateX(float angle)
{
    mRoll += angle;
    mTransform = glm::rotate(mTransform, angle, glm::vec3(1, 0, 0));
}

void Transform::RotateY(float angle)
{
    mPitch += angle;
    mTransform = glm::rotate(mTransform, angle, glm::vec3(0, 1, 0));
}

void Transform::RotateZ(float angle)
{
    mYaw += angle;
    mTransform = glm::rotate(mTransform, angle, glm::vec3(0, 0, 1));
}

void Transform::Scale(const glm::vec3& scale)
{
    mScale *= scale;
    mTransform = glm::scale(mTransform, scale);
}

void Transform::SetParent(Transform* par)
{
    mParent = par;
}

