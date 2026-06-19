#pragma once

class Camera
{
public:
    void Follow(float targetX, float targetY);
    float GetX() const;
    float GetY() const;

private:
    float x = 0.0f;
    float y = 0.0f;
};
