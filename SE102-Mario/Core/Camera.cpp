#include "Camera.h"

void Camera::Follow(float targetX, float targetY)
{
    x = targetX;
    y = targetY;
}

float Camera::GetX() const
{
    return x;
}

float Camera::GetY() const
{
    return y;
}
