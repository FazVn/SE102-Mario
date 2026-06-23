#pragma once

#include "../GameObject.h"

class Sprite;

class Platform : public GameObject
{
public:
    Platform();
    Platform(float x, float y, float width, float height, const Sprite* sprite, int tileWidth, int tileHeight);

    void SetSprite(const Sprite* sprite);
    void SetTileSize(int width, int height);
    void Render(Renderer& renderer) override;

private:
    const Sprite* sprite = nullptr;
    int tileWidth = 16;
    int tileHeight = 16;
};
