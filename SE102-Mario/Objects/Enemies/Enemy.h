#pragma once

#include "../GameObject.h"

#include <vector>

class Renderer;
class Sprite;

class Enemy : public GameObject
{
public:
    Enemy(float x, float y, float width, float height, float velocityX, const Sprite* walkFrame1, const Sprite* walkFrame2,
        bool walkFramesFaceRight = false, bool turnAroundAtLedges = false);

    void Update(float deltaTime, const std::vector<RectF>& solidBounds, float levelHeight);
    void RenderAt(Renderer& renderer, float offsetX, float offsetY);
    void Defeat();

private:
    const Sprite* GetCurrentFrame() const;

    const Sprite* walkFrame1 = nullptr;
    const Sprite* walkFrame2 = nullptr;
    bool walkFramesFaceRight = false;
    bool turnAroundAtLedges = false;
    float animationTime = 0.0f;
};
