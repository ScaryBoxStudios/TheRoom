#include "Camera.hpp"

Camera::Camera()
{
    mState.position = glm::vec3(0, 0, 0);
    mState.front = glm::vec3(0, 0, -1);
    mState.up = glm::vec3(0, 1, 0);
    mState.yaw = -90.0f;
    mState.pitch = 0.0f;

    mProperties.movementSpeed = 0.3f;
    mProperties.sensitivity = 0.05f;
    mProperties.pitchLim = 90.0f;
}

Camera::~Camera()
{
}

void Camera::SetPos(const glm::vec3& pos)
{
    mState.position = pos;
}

void Camera::Move(std::vector<MoveDirection> md)
{
    mState = CalcMove(mState, mProperties, md, 1.0f);
}

void Camera::Look(std::tuple<float, float> lookOffset)
{
    mState = CalcLook(mState, mProperties, lookOffset, 1.0f);
}

auto Camera::Interpolate(std::vector<MoveDirection> md, std::tuple<float, float> lookOffset, float interpolation) -> CameraState
{
    auto s = CalcMove(mState, mProperties, md, interpolation);
    return CalcLook(s, mProperties, lookOffset, interpolation);
}

auto Camera::CalcMove(const CameraState& prevState, const CameraProperties& camProps, std::vector<MoveDirection> md, float interpolation) -> CameraState
{
    CameraState newState = prevState;

    float interpolatedMovementSpeed = camProps.movementSpeed * interpolation;
    auto& pos = newState.position;
    auto& front = newState.front;
    auto& up = newState.up;

    for (const auto& m : md)
    {
        switch(m)
        {
            case MoveDirection::Forward:
                pos += front * interpolatedMovementSpeed;
                break;
            case MoveDirection::Left:
                pos -= glm::normalize(glm::cross(front, up)) * interpolatedMovementSpeed;
                break;
            case MoveDirection::BackWard:
                pos -= front * interpolatedMovementSpeed;
                break;
            case MoveDirection::Right:
                pos += glm::normalize(glm::cross(front, up)) * interpolatedMovementSpeed;
                break;
        }
    }

    return newState;
}

auto Camera::CalcLook(const CameraState& prevState, const CameraProperties& camProps, std::tuple<float, float> lookOffset, float interpolation) -> CameraState
{
    CameraState newState = prevState;

    float xOffset = std::get<0>(lookOffset) * interpolation;
    float yOffset = std::get<1>(lookOffset) * interpolation;
    float sensitivity = camProps.sensitivity;
    float pitchLim = camProps.pitchLim;
    float& yaw = newState.yaw;
    float& pitch = newState.pitch;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch -= yOffset;

    if(pitch > pitchLim)
        pitch = pitchLim;
    if(pitch < -pitchLim)
        pitch = -pitchLim;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    newState.front = glm::normalize(front);

    return newState;
}
