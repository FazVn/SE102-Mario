#pragma once

#include "Enemy.h"

class Goomba : public Enemy
{
public:
    Goomba(float x, float y, const Sprite* walkFrame1, const Sprite* walkFrame2);
};
