#pragma once

#include "../GameObject.h"

#include <vector>

class Renderer;
class Sprite;

class Star : public GameObject
{
public:
    Star(float x, float y, const Sprite* yellowFrame, const Sprite* greenFrame, const Sprite* redFrame);

    void Update(float deltaTime, const std::vector<RectF>& solidBounds, float levelHeight);
    void RenderAt(Renderer& renderer, float offsetX, float offsetY);

private:
    const Sprite* GetCurrentFrame() const;

    const Sprite* yellowFrame = nullptr;
    const Sprite* greenFrame = nullptr;
    const Sprite* redFrame = nullptr;
    float animationTime = 0.0f;
};
