#pragma once

#include "../GameObject.h"

class Sprite;

class Mario : public GameObject
{
public:
    static constexpr int SmallStandXLeft = 3;
    static constexpr int SmallStandYTop = 35;
    static constexpr int SmallStandXRight = 23;
    static constexpr int SmallStandYDown = 66;
    static constexpr int DefaultRenderWidth = 48;
    static constexpr int DefaultRenderHeight = 48;

    Mario();
    Mario(float x, float y, const Sprite* sprite, int renderWidth = DefaultRenderWidth, int renderHeight = DefaultRenderHeight);

    void SetSprite(const Sprite* sprite);
    void SetRenderSize(int renderWidth, int renderHeight);
    void Render(Renderer& renderer) override;

private:
    const Sprite* sprite = nullptr;
    int renderWidth = DefaultRenderWidth;
    int renderHeight = DefaultRenderHeight;
};
