#pragma once

#include "Enemy.h"

class Koopa : public Enemy
{
public:
    Koopa(float x, float y, const Sprite* walkFrame1, const Sprite* walkFrame2, bool turnAroundAtLedges = false);
};
