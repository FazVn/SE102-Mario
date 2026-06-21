#pragma once

#include <vector>

class Sprite;

struct AnimationFrame
{
    const Sprite* sprite = nullptr;
    float duration = 0.1f;
};

class Animation
{
public:
    void AddFrame(const Sprite* sprite, float duration);
    void Clear();
    void Reset();
    void Update(float deltaTime);

    const Sprite* GetCurrentSprite() const;
    bool IsEmpty() const;
    void SetLooping(bool value);

private:
    std::vector<AnimationFrame> frames;
    size_t currentFrame = 0;
    float currentFrameTime = 0.0f;
    bool looping = true;
};
