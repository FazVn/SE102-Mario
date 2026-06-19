#pragma once

class GameObject
{
public:
    virtual ~GameObject() = default;
    virtual void Update(float deltaTime);
};
