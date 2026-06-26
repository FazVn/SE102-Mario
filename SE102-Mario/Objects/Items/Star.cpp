#include "Star.h"

#include "../../Core/Renderer.h"
#include "../../Core/Sprite.h"

#include <cmath>

namespace
{
    constexpr float StarSize = 32.0f;
    constexpr float StarSpeed = 120.0f;
    constexpr float StarGravityPixels = 980.0f;
    constexpr float StarBounceSpeed = -360.0f;
    constexpr float FrameDuration = 0.08f;

    bool HasHorizontalOverlap(const RectF& first, const RectF& second)
    {
        return first.left < second.right && first.right > second.left;
    }
}

Star::Star(float x, float y, const Sprite* yellowFrame, const Sprite* greenFrame, const Sprite* redFrame)
    : GameObject(x, y, StarSize, StarSize),
    yellowFrame(yellowFrame),
    greenFrame(greenFrame),
    redFrame(redFrame)
{
    SetVelocity(StarSpeed, -220.0f);
    SetCollidable(true);
}

void Star::Update(float deltaTime, const std::vector<RectF>& solidBounds, float levelHeight)
{
    if (!IsActive())
    {
        return;
    }

    animationTime += deltaTime;
    velocityY += StarGravityPixels * deltaTime;

    const float previousX = x;
    x += velocityX * deltaTime;
    RectF bounds = GetBoundingBox();
    for (const RectF& solid : solidBounds)
    {
        if (bounds.Intersects(solid))
        {
            x = previousX;
            velocityX = -velocityX;
            break;
        }
    }

    const float previousY = y;
    y += velocityY * deltaTime;
    bounds = GetBoundingBox();

    bool landed = false;
    float landingTop = 0.0f;
    if (velocityY >= 0.0f)
    {
        for (const RectF& solid : solidBounds)
        {
            const bool crossedTop = previousY + height <= solid.top && bounds.bottom >= solid.top;
            if (HasHorizontalOverlap(bounds, solid) && crossedTop)
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
        velocityY = StarBounceSpeed;
    }

    if (y > levelHeight + 128.0f)
    {
        SetActive(false);
    }
}

void Star::RenderAt(Renderer& renderer, float offsetX, float offsetY)
{
    const Sprite* frame = GetCurrentFrame();
    if (!IsVisible() || !frame)
    {
        return;
    }

    renderer.DrawSprite(*frame,
        static_cast<int>(std::lround(x - offsetX)),
        static_cast<int>(std::lround(y - offsetY)),
        static_cast<int>(std::lround(width)),
        static_cast<int>(std::lround(height)));
}

const Sprite* Star::GetCurrentFrame() const
{
    const int frameIndex = static_cast<int>(animationTime / FrameDuration) % 3;
    if (frameIndex == 1)
    {
        return greenFrame ? greenFrame : yellowFrame;
    }
    if (frameIndex == 2)
    {
        return redFrame ? redFrame : yellowFrame;
    }

    return yellowFrame;
}
