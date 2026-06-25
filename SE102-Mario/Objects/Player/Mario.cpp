#include "Mario.h"

#include "../../Core/Input.h"
#include "../../Core/RenderOptions.h"
#include "../../Core/Renderer.h"
#include "../../Core/Sprite.h"

#include <algorithm>
#include <cmath>

namespace
{
    float PixelsToMeters(float pixels)
    {
        return pixels * Mario::PixelToMeter;
    }

    float MetersToPixels(float meters)
    {
        return meters * Mario::MeterToPixel;
    }

    float MoveTowards(float current, float target, float maxDelta)
    {
        if (current < target)
        {
            const float next = current + maxDelta;
            return next > target ? target : next;
        }

        if (current > target)
        {
            const float next = current - maxDelta;
            return next < target ? target : next;
        }

        return target;
    }

    float CalculateJumpVelocity(float marioHeightMeters)
    {
        const float jumpHeight = marioHeightMeters * Mario::JumpHeightMultiplier;
        return std::sqrt(2.0f * Mario::Gravity * jumpHeight);
    }

    bool HasHorizontalOverlap(const RectF& first, const RectF& second)
    {
        return first.left < second.right && first.right > second.left;
    }

    bool HasVerticalOverlap(const RectF& first, const RectF& second)
    {
        return first.top < second.bottom && first.bottom > second.top;
    }
}

Mario::Mario()
{
    SetCollidable(true);
    SetRenderSize(DefaultRenderWidth, DefaultRenderHeight);
    SetPosition(0.0f, 0.0f);
}

Mario::Mario(float x, float y, const Sprite* sprite, int renderWidth, int renderHeight)
    : GameObject(x, y, 0.0f, 0.0f)
{
    SetCollidable(true);
    SetSprite(sprite);
    SetRenderSize(renderWidth, renderHeight);
    SetPosition(x, y);
}

void Mario::SetSprite(const Sprite* newSprite)
{
    sprite = newSprite;
}

void Mario::SetRenderSize(int newRenderWidth, int newRenderHeight)
{
    renderWidth = newRenderWidth > 0 ? newRenderWidth : 0;
    renderHeight = newRenderHeight > 0 ? newRenderHeight : 0;
    SetSize(static_cast<float>(renderWidth), static_cast<float>(renderHeight));
    SyncPhysicsFromPixels();
}

void Mario::SetPosition(float newX, float newY)
{
    GameObject::SetPosition(newX, newY);
    SyncPhysicsFromPixels();
    previousPixelX = newX;
    previousPixelY = newY;
}

void Mario::SetOnGround(bool value)
{
    onGround = value;

    if (onGround)
    {
        velocityMetersY = 0.0f;
    }

    SyncPixelsFromPhysics();
    UpdateState();
}

void Mario::Update(const Input& input, float deltaTime)
{
    previousPixelX = x;
    previousPixelY = y;

    const bool moveLeft = input.IsKeyDown(VK_LEFT) || input.IsKeyDown('A');
    const bool moveRight = input.IsKeyDown(VK_RIGHT) || input.IsKeyDown('D');
    float inputDirection = 0.0f;

    if (moveLeft)
    {
        inputDirection -= 1.0f;
    }

    if (moveRight)
    {
        inputDirection += 1.0f;
    }

    if (inputDirection == 0.0f)
    {
        velocityMetersX = MoveTowards(velocityMetersX, 0.0f, RunDeceleration * deltaTime);
    }
    else
    {
        facingDirection = inputDirection < 0.0f ? FacingDirection::Left : FacingDirection::Right;

        const bool braking = (velocityMetersX > 0.0f && inputDirection < 0.0f) ||
            (velocityMetersX < 0.0f && inputDirection > 0.0f);

        if (braking)
        {
            const float activeDeceleration = RunDeceleration + RunAcceleration;
            velocityMetersX = MoveTowards(velocityMetersX, 0.0f, activeDeceleration * deltaTime);
        }
        else
        {
            velocityMetersX += inputDirection * RunAcceleration * deltaTime;
            velocityMetersX = std::clamp(velocityMetersX, -MaxRunSpeed, MaxRunSpeed);
        }
    }

    if (input.WasKeyPressed(VK_SPACE) && onGround)
    {
        velocityMetersY = -CalculateJumpVelocity(PixelsToMeters(height));
        onGround = false;
    }

    velocityMetersY += Gravity * deltaTime;
    positionMetersX += velocityMetersX * deltaTime;
    positionMetersY += velocityMetersY * deltaTime;

    SyncPixelsFromPhysics();
    UpdateState();
}

void Mario::ResolveSolidCollisions(const std::vector<RectF>& solidBounds)
{
    RectF marioBounds = GetBoundingBox();
    const float previousTop = previousPixelY;
    const float previousBottom = previousPixelY + height;
    bool landed = false;
    float landingTop = 0.0f;
    bool hitCeiling = false;
    float ceilingBottom = 0.0f;

    if (velocityMetersY >= 0.0f)
    {
        for (const RectF& solidBoundsItem : solidBounds)
        {
            const bool overlapsHorizontally = HasHorizontalOverlap(marioBounds, solidBoundsItem);
            const bool crossedPlatformTop = previousBottom <= solidBoundsItem.top &&
                marioBounds.bottom >= solidBoundsItem.top;
            const bool isAbovePlatformBottom = marioBounds.top < solidBoundsItem.bottom;

            if (overlapsHorizontally && crossedPlatformTop && isAbovePlatformBottom)
            {
                if (!landed || solidBoundsItem.top < landingTop)
                {
                    landed = true;
                    landingTop = solidBoundsItem.top;
                }
            }
        }
    }
    else
    {
        for (const RectF& solidBoundsItem : solidBounds)
        {
            const bool overlapsHorizontally = HasHorizontalOverlap(marioBounds, solidBoundsItem);
            const bool crossedPlatformBottom = previousTop >= solidBoundsItem.bottom &&
                marioBounds.top <= solidBoundsItem.bottom;
            const bool isBelowPlatformTop = marioBounds.bottom > solidBoundsItem.top;

            if (overlapsHorizontally && crossedPlatformBottom && isBelowPlatformTop)
            {
                if (!hitCeiling || solidBoundsItem.bottom > ceilingBottom)
                {
                    hitCeiling = true;
                    ceilingBottom = solidBoundsItem.bottom;
                }
            }
        }
    }

    if (landed)
    {
        positionMetersY = PixelsToMeters(landingTop - height);
        velocityMetersY = 0.0f;
        onGround = true;
    }
    else if (hitCeiling)
    {
        positionMetersY = PixelsToMeters(ceilingBottom);
        velocityMetersY = 0.0f;
        onGround = false;
    }
    else
    {
        onGround = false;
    }

    SyncPixelsFromPhysics();
    marioBounds = GetBoundingBox();

    const float previousLeft = previousPixelX;
    const float previousRight = previousPixelX + width;
    bool hitWall = false;
    float wallEdge = 0.0f;

    if (velocityMetersX > 0.0f)
    {
        for (const RectF& solidBoundsItem : solidBounds)
        {
            const bool overlapsVertically = HasVerticalOverlap(marioBounds, solidBoundsItem);
            const bool crossedLeftEdge = previousRight <= solidBoundsItem.left &&
                marioBounds.right >= solidBoundsItem.left;

            if (overlapsVertically && crossedLeftEdge)
            {
                if (!hitWall || solidBoundsItem.left < wallEdge)
                {
                    hitWall = true;
                    wallEdge = solidBoundsItem.left;
                }
            }
        }

        if (hitWall)
        {
            positionMetersX = PixelsToMeters(wallEdge - width);
            velocityMetersX = 0.0f;
        }
    }
    else if (velocityMetersX < 0.0f)
    {
        for (const RectF& solidBoundsItem : solidBounds)
        {
            const bool overlapsVertically = HasVerticalOverlap(marioBounds, solidBoundsItem);
            const bool crossedRightEdge = previousLeft >= solidBoundsItem.right &&
                marioBounds.left <= solidBoundsItem.right;

            if (overlapsVertically && crossedRightEdge)
            {
                if (!hitWall || solidBoundsItem.right > wallEdge)
                {
                    hitWall = true;
                    wallEdge = solidBoundsItem.right;
                }
            }
        }

        if (hitWall)
        {
            positionMetersX = PixelsToMeters(wallEdge);
            velocityMetersX = 0.0f;
        }
    }

    if (hitWall)
    {
        SyncPixelsFromPhysics();
    }

    UpdateState();
}

void Mario::ResolveVerticalCollision(const std::vector<RectF>& solidBounds)
{
    ResolveSolidCollisions(solidBounds);
}

void Mario::Render(Renderer& renderer)
{
    if (!IsVisible() || !sprite || renderWidth <= 0 || renderHeight <= 0)
    {
        return;
    }

    RenderOptions options;
    options.flipX = facingDirection == FacingDirection::Left;
    renderer.DrawSprite(*sprite, static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y)), renderWidth, renderHeight, options);
}

bool Mario::IsOnGround() const
{
    return onGround;
}

Mario::FacingDirection Mario::GetFacingDirection() const
{
    return facingDirection;
}

Mario::State Mario::GetState() const
{
    return state;
}

void Mario::SyncPixelsFromPhysics()
{
    GameObject::SetPosition(MetersToPixels(positionMetersX), MetersToPixels(positionMetersY));
    GameObject::SetVelocity(MetersToPixels(velocityMetersX), MetersToPixels(velocityMetersY));
}

void Mario::SyncPhysicsFromPixels()
{
    positionMetersX = PixelsToMeters(x);
    positionMetersY = PixelsToMeters(y);
}

void Mario::UpdateState()
{
    if (!onGround)
    {
        state = velocityMetersY < 0.0f ? State::Jump : State::Fall;
        return;
    }

    state = std::abs(velocityMetersX) > 0.05f ? State::Walk : State::Idle;
}
