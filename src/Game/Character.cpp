#include "Character.hpp"
#include <cmath>

#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

void Character::Init(Window* const window, Scene* const scene)
{
    mWindow = window;
    mScene  = scene;

    for(auto& p : mScene->GetNodes())
    {
        if(p.first == "character")
            mCharacter = p.second.get();
    }
}

void Character::Update()
{
    if(mWindow->IsKeyPressed(Key::A))
    {
        mRotation += mStats.rotationSpeed;
        mScene->Rotate(mCharacter, RotationAxis::Y, mStats.rotationSpeed);
    } 

    if(mWindow->IsKeyPressed(Key::D))
    {
        mRotation -= mStats.rotationSpeed;
        mScene->Rotate(mCharacter, RotationAxis::Y, -mStats.rotationSpeed);
    }

    if(mWindow->IsKeyPressed(Key::S))
        mScene->Move(mCharacter, glm::vec3(std::sin(glm::radians(mRotation)) * mStats.speed, 0, std::cos(glm::radians(mRotation)) * mStats.speed));
    if(mWindow->IsKeyPressed(Key::W))
        mScene->Move(mCharacter, glm::vec3(-(std::sin(glm::radians(mRotation)) * mStats.speed), 0, -(std::cos(glm::radians(mRotation)) * mStats.speed)));
}

SceneNode* Character::GetCharacterNode() const
{
    return mCharacter;
}
