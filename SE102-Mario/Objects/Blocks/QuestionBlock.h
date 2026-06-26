#pragma once

#include "../GameObject.h"

class Renderer;
class Sprite;

class QuestionBlock : public GameObject
{
public:
    enum class Content
    {
        Coin,
        Mushroom,
        Star
    };

    QuestionBlock(float x, float y, const Sprite* frame1, const Sprite* frame2, const Sprite* frame3, const Sprite* usedFrame, Content content);

    void Update(float deltaTime) override;
    void RenderAt(Renderer& renderer, float offsetX, float offsetY);
    bool Hit();

    bool IsUsed() const;
    Content GetContent() const;

private:
    const Sprite* GetCurrentFrame() const;

    const Sprite* frame1 = nullptr;
    const Sprite* frame2 = nullptr;
    const Sprite* frame3 = nullptr;
    const Sprite* usedFrame = nullptr;
    Content content = Content::Coin;
    float animationTime = 0.0f;
    float bumpTime = 0.0f;
    bool used = false;
};
