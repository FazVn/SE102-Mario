#include "Koopa.h"

Koopa::Koopa(float x, float y, const Sprite* walkFrame1, const Sprite* walkFrame2, bool turnAroundAtLedges)
    : Enemy(x, y, 32.0f, 48.0f, -36.0f, walkFrame1, walkFrame2, true, turnAroundAtLedges)
{
}
