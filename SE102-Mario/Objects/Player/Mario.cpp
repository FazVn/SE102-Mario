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

    float MoveTowardZero(float value, float maxDelta)
    {
        if (value > maxDelta)
        {
            return value - maxDelta;
        }

        if (value < -maxDelta)
        {
            return value + maxDelta;
        }

        return 0.0f;
    }

    float CalculateJumpVelocity(float marioHeightMeters)
    {
        const float jumpHeight = marioHeightMeters * Mario::JumpHeightMultiplier;
        return std::sqrt(2.0f * Mario::Gravity * jumpHeight);
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
    previousPixelY = y;

    const bool moveLeft = input.IsKeyDown(VK_LEFT) || input.IsKeyDown('A');
    const bool moveRight = input.IsKeyDown(VK_RIGHT) || input.IsKeyDown('D');
    const int horizontalDirection = (moveRight ? 1 : 0) - (moveLeft ? 1 : 0);

    if (horizontalDirection != 0)
    {
        velocityMetersX += horizontalDirection * RunAcceleration * deltaTime;
        facingDirection = horizontalDirection < 0 ? FacingDirection::Left : FacingDirection::Right;
    }
    else
    {
        velocityMetersX = MoveTowardZero(velocityMetersX, RunDeceleration * deltaTime);
    }

    velocityMetersX = std::clamp(velocityMetersX, -MaxRunSpeed, MaxRunSpeed);

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

void Mario::ResolveVerticalCollision(const std::vector<RectF>& solidBounds)
{
    const RectF marioBounds = GetBoundingBox();
    const float previousBottom = previousPixelY + height;
    bool landed = false;
    float landingTop = 0.0f;

    if (velocityMetersY >= 0.0f)
    {
        for (const RectF& solidBoundsItem : solidBounds)
        {
            const bool overlapsHorizontally = marioBounds.left < solidBoundsItem.right &&
                marioBounds.right > solidBoundsItem.left;
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

    if (landed)
    {
        positionMetersY = PixelsToMeters(landingTop - height);
        velocityMetersY = 0.0f;
        onGround = true;
    }
    else
    {
        onGround = false;
    }

    SyncPixelsFromPhysics();
    UpdateState();
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
