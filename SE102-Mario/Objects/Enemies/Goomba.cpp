#include "Goomba.h"

Goomba::Goomba(float x, float y, const Sprite* walkFrame1, const Sprite* walkFrame2)
    : Enemy(x, y, 32.0f, 32.0f, -42.0f, walkFrame1, walkFrame2)
{
}
