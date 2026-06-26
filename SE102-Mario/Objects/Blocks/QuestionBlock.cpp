#include "QuestionBlock.h"

#include "../../Core/Renderer.h"
#include "../../Core/Sprite.h"

#include <cmath>

namespace
{
    constexpr float BlockSize = 32.0f;
    constexpr float FrameDuration = 0.14f;
    constexpr float BumpDuration = 0.24f;
    constexpr float BumpHeight = 10.0f;
}

QuestionBlock::QuestionBlock(float x, float y, const Sprite* frame1, const Sprite* frame2, const Sprite* frame3, const Sprite* usedFrame, Content content)
    : GameObject(x, y, BlockSize, BlockSize),
    frame1(frame1),
    frame2(frame2),
    frame3(frame3),
    usedFrame(usedFrame),
    content(content)
{
    SetSolid(true);
}

void QuestionBlock::Update(float deltaTime)
{
    animationTime += deltaTime;

    if (bumpTime > 0.0f)
    {
        bumpTime -= deltaTime;
        if (bumpTime < 0.0f)
        {
            bumpTime = 0.0f;
        }
    }
}

void QuestionBlock::RenderAt(Renderer& renderer, float offsetX, float offsetY)
{
    const Sprite* frame = GetCurrentFrame();
    if (!IsVisible() || !frame)
    {
        return;
    }

    float bumpOffset = 0.0f;
    if (bumpTime > 0.0f)
    {
        const float progress = 1.0f - bumpTime / BumpDuration;
        bumpOffset = -std::sin(progress * 3.14159265f) * BumpHeight;
    }

    renderer.DrawSprite(*frame,
        static_cast<int>(std::lround(x - offsetX)),
        static_cast<int>(std::lround(y + bumpOffset - offsetY)),
        static_cast<int>(std::lround(width)),
        static_cast<int>(std::lround(height)));
}

bool QuestionBlock::Hit()
{
    if (used)
    {
        return false;
    }

    used = true;
    bumpTime = BumpDuration;
    return true;
}

bool QuestionBlock::IsUsed() const
{
    return used;
}

QuestionBlock::Content QuestionBlock::GetContent() const
{
    return content;
}

const Sprite* QuestionBlock::GetCurrentFrame() const
{
    if (used)
    {
        return usedFrame;
    }

    const int frameIndex = static_cast<int>(animationTime / FrameDuration) % 3;
    if (frameIndex == 1)
    {
        return frame2 ? frame2 : frame1;
    }
    if (frameIndex == 2)
    {
        return frame3 ? frame3 : frame1;
    }

    return frame1;
}
