#include "Animation.h"

#include <algorithm>

void Animation::AddFrame(const Sprite* sprite, float duration)
{
    if (!sprite)
    {
        return;
    }

    frames.push_back(AnimationFrame{ sprite, std::max(duration, 0.001f) });
}

void Animation::Clear()
{
    frames.clear();
    Reset();
}

void Animation::Reset()
{
    currentFrame = 0;
    currentFrameTime = 0.0f;
}

void Animation::Update(float deltaTime)
{
    if (frames.empty())
    {
        return;
    }

    currentFrameTime += deltaTime;
    while (currentFrameTime >= frames[currentFrame].duration)
    {
        currentFrameTime -= frames[currentFrame].duration;

        if (currentFrame + 1 < frames.size())
        {
            ++currentFrame;
            continue;
        }

        if (looping)
        {
            currentFrame = 0;
        }
        else
        {
            currentFrame = frames.size() - 1;
            currentFrameTime = 0.0f;
            break;
        }
    }
}

const Sprite* Animation::GetCurrentSprite() const
{
    if (frames.empty())
    {
        return nullptr;
    }

    return frames[currentFrame].sprite;
}

bool Animation::IsEmpty() const
{
    return frames.empty();
}

void Animation::SetLooping(bool value)
{
    looping = value;
}
