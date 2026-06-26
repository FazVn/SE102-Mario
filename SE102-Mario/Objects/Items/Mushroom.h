#pragma once

#include "../GameObject.h"

#include <vector>

class Renderer;
class Sprite;

class Mushroom : public GameObject
{
public:
    Mushroom(float x, float y, const Sprite* sprite);

    void Update(float deltaTime, const std::vector<RectF>& solidBounds, float levelHeight);
    void RenderAt(Renderer& renderer, float offsetX, float offsetY);

private:
    const Sprite* sprite = nullptr;
};
