#include "Character.hpp"
#include <cmath>

void Character::Init(Window* const window, Scene* const scene, glm::vec3 pos /* = {0, 0, 0} */)
{
    mWindow = window;
    mScene  = scene;

    for(auto& p : mScene->GetNodes())
    {
        if(p.first == "character")
            mCharacter = p.second.get();
    }

    mPosition = pos;
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

    auto moveChar = [this](bool moveForward)
    {
        float rad = glm::radians(mRotation);
        double mx = std::sin(rad) * mStats.speed;
        double mz = std::cos(rad) * mStats.speed;

        if(moveForward)
        {
            mx = -mx;
            mz = -mz;
        }

        glm::vec3 move(mx, 0, mz);
        mScene->Move(mCharacter, move);
        mPosition += move;
    };

    if(mWindow->IsKeyPressed(Key::S))
        moveChar(false);
    if(mWindow->IsKeyPressed(Key::W))
        moveChar(true);
}

SceneNode* Character::GetCharacterNode() const
{
    return mCharacter;
}

float Character::GetRotation() const
{
    return mRotation;
}

const glm::vec3& Character::GetPosition() const
{
    return mPosition;
}
