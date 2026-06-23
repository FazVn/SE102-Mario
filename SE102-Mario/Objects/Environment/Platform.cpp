#include "Platform.h"

#include "../../Core/Renderer.h"
#include "../../Core/Sprite.h"

#include <cmath>

Platform::Platform()
{
    SetCollidable(true);
    SetSolid(true);
}

Platform::Platform(float x, float y, float width, float height, const Sprite* sprite, int tileWidth, int tileHeight)
    : GameObject(x, y, width, height)
{
    SetCollidable(true);
    SetSolid(true);
    SetSprite(sprite);
    SetTileSize(tileWidth, tileHeight);
}

void Platform::SetSprite(const Sprite* newSprite)
{
    sprite = newSprite;
}

void Platform::SetTileSize(int newTileWidth, int newTileHeight)
{
    tileWidth = newTileWidth > 0 ? newTileWidth : 16;
    tileHeight = newTileHeight > 0 ? newTileHeight : 16;
}

void Platform::Render(Renderer& renderer)
{
    if (!IsVisible() || !sprite || tileWidth <= 0 || tileHeight <= 0)
    {
        return;
    }

    const int drawX = static_cast<int>(std::lround(x));
    const int drawY = static_cast<int>(std::lround(y));
    const int drawWidth = static_cast<int>(std::lround(width));
    const int drawHeight = static_cast<int>(std::lround(height));

    for (int yOffset = 0; yOffset + tileHeight <= drawHeight; yOffset += tileHeight)
    {
        for (int xOffset = 0; xOffset + tileWidth <= drawWidth; xOffset += tileWidth)
        {
            renderer.DrawSprite(*sprite, drawX + xOffset, drawY + yOffset, tileWidth, tileHeight);
        }
    }
}
