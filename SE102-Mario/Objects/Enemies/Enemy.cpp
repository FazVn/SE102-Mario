#include "Enemy.h"

#include "../../Core/RenderOptions.h"
#include "../../Core/Renderer.h"
#include "../../Core/Sprite.h"

#include <cmath>

namespace
{
    constexpr float EnemyGravityPixels = 980.0f;
    constexpr float WalkFrameDuration = 0.16f;
    constexpr float LedgeProbeDistance = 2.0f;
    constexpr float GroundProbeTolerance = 4.0f;

    bool HasHorizontalOverlap(const RectF& first, const RectF& second)
    {
        return first.left < second.right && first.right > second.left;
    }

    bool HasGroundAt(float probeX, float feetY, const std::vector<RectF>& solidBounds)
    {
        for (const RectF& solid : solidBounds)
        {
            const bool probeInsideSolid = probeX >= solid.left && probeX <= solid.right;
            const bool surfaceAtFeet = std::abs(solid.top - feetY) <= GroundProbeTolerance;
            if (probeInsideSolid && surfaceAtFeet)
            {
                return true;
            }
        }

        return false;
    }
}

Enemy::Enemy(float x, float y, float width, float height, float velocityX, const Sprite* walkFrame1, const Sprite* walkFrame2,
    bool walkFramesFaceRight, bool turnAroundAtLedges)
    : GameObject(x, y, width, height),
    walkFrame1(walkFrame1),
    walkFrame2(walkFrame2),
    walkFramesFaceRight(walkFramesFaceRight),
    turnAroundAtLedges(turnAroundAtLedges)
{
    SetVelocity(velocityX, 0.0f);
    SetCollidable(true);
}

void Enemy::Update(float deltaTime, const std::vector<RectF>& solidBounds, float levelHeight)
{
    if (!IsActive())
    {
        return;
    }

    animationTime += deltaTime;

    if (turnAroundAtLedges && std::abs(velocityY) <= GroundProbeTolerance && velocityX != 0.0f)
    {
        const RectF currentBounds = GetBoundingBox();
        bool isStandingOnGround = false;
        for (const RectF& solid : solidBounds)
        {
            const bool surfaceAtFeet = std::abs(solid.top - currentBounds.bottom) <= GroundProbeTolerance;
            if (surfaceAtFeet && HasHorizontalOverlap(currentBounds, solid))
            {
                isStandingOnGround = true;
                break;
            }
        }

        if (isStandingOnGround)
        {
            const float probeX = velocityX < 0.0f
                ? currentBounds.left - LedgeProbeDistance
                : currentBounds.right + LedgeProbeDistance;
            if (!HasGroundAt(probeX, currentBounds.bottom, solidBounds))
            {
                velocityX = -velocityX;
            }
        }
    }

    velocityY += EnemyGravityPixels * deltaTime;

    const float previousX = x;
    x += velocityX * deltaTime;
    RectF enemyBounds = GetBoundingBox();

    for (const RectF& solid : solidBounds)
    {
        if (enemyBounds.Intersects(solid))
        {
            x = previousX;
            velocityX = -velocityX;
            break;
        }
    }

    const float previousY = y;
    y += velocityY * deltaTime;
    enemyBounds = GetBoundingBox();

    bool landed = false;
    float landingTop = 0.0f;
    if (velocityY >= 0.0f)
    {
        for (const RectF& solid : solidBounds)
        {
            const bool crossedTop = previousY + height <= solid.top && enemyBounds.bottom >= solid.top;
            if (HasHorizontalOverlap(enemyBounds, solid) && crossedTop)
            {
                if (!landed || solid.top < landingTop)
                {
                    landed = true;
                    landingTop = solid.top;
                }
            }
        }
    }

    if (landed)
    {
        y = landingTop - height;
        velocityY = 0.0f;
    }

    if (y > levelHeight + 128.0f)
    {
        SetActive(false);
    }
}

void Enemy::RenderAt(Renderer& renderer, float offsetX, float offsetY)
{
    if (!IsVisible())
    {
        return;
    }

    const Sprite* frame = GetCurrentFrame();
    if (!frame)
    {
        return;
    }

    RenderOptions options;
    options.flipX = walkFramesFaceRight && velocityX < 0.0f;
    renderer.DrawSprite(*frame,
        static_cast<int>(std::lround(x - offsetX)),
        static_cast<int>(std::lround(y - offsetY)),
        static_cast<int>(std::lround(width)),
        static_cast<int>(std::lround(height)),
        options);
}

void Enemy::Defeat()
{
    SetActive(false);
}

const Sprite* Enemy::GetCurrentFrame() const
{
    const Sprite* frame = (static_cast<int>(animationTime / WalkFrameDuration) % 2 == 0)
        ? walkFrame1
        : walkFrame2;

    if (!frame)
    {
        frame = walkFrame1 ? walkFrame1 : walkFrame2;
    }

    return frame;
}
