#include "Coin.h"

#include "../../Core/Renderer.h"
#include "../../Core/Sprite.h"

#include <cmath>

namespace
{
    constexpr float CoinWidth = 24.0f;
    constexpr float CoinHeight = 24.0f;
    constexpr float FrameDuration = 0.1f;
    constexpr float PopupDuration = 0.55f;
    constexpr float PopupSpeed = -150.0f;
    constexpr float PopupGravity = 520.0f;
}

Coin::Coin(float x, float y, const Sprite* frame1, const Sprite* frame2, const Sprite* frame3, const Sprite* frame4, bool popup)
    : GameObject(x, y, CoinWidth, CoinHeight),
    frame1(frame1),
    frame2(frame2),
    frame3(frame3),
    frame4(frame4),
    popup(popup)
{
    SetCollidable(!popup);
    if (popup)
    {
        SetVelocity(0.0f, PopupSpeed);
    }
}

void Coin::Update(float deltaTime)
{
    animationTime += deltaTime;

    if (popup)
    {
        lifeTime += deltaTime;
        velocityY += PopupGravity * deltaTime;
        GameObject::Update(deltaTime);

        if (lifeTime >= PopupDuration)
        {
            SetActive(false);
        }
    }
}

void Coin::RenderAt(Renderer& renderer, float offsetX, float offsetY)
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

bool Coin::IsFinished() const
{
    return !IsActive();
}

const Sprite* Coin::GetCurrentFrame() const
{
    static const int frameSequence[] = { 1, 2, 3, 4, 3, 2 };
    const int frameIndex = static_cast<int>(animationTime / FrameDuration) % 6;
    switch (frameSequence[frameIndex])
    {
    case 2:
        return frame2 ? frame2 : frame1;
    case 3:
        return frame3 ? frame3 : frame1;
    case 4:
        return frame4 ? frame4 : frame1;
    default:
        return frame1;
    }
}
