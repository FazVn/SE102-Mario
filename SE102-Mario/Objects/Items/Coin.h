#pragma once

#include "../GameObject.h"

class Renderer;
class Sprite;

class Coin : public GameObject
{
public:
    Coin(float x, float y, const Sprite* frame1, const Sprite* frame2, const Sprite* frame3, const Sprite* frame4, bool popup = false);

    void Update(float deltaTime) override;
    void RenderAt(Renderer& renderer, float offsetX, float offsetY);
    bool IsFinished() const;

private:
    const Sprite* GetCurrentFrame() const;

    const Sprite* frame1 = nullptr;
    const Sprite* frame2 = nullptr;
    const Sprite* frame3 = nullptr;
    const Sprite* frame4 = nullptr;
    float animationTime = 0.0f;
    float lifeTime = 0.0f;
    bool popup = false;
};
