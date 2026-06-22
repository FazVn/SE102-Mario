#include "Mario.h"

#include "../../Core/Renderer.h"
#include "../../Core/Sprite.h"

#include <cmath>

Mario::Mario()
{
    SetCollidable(false);
    SetRenderSize(DefaultRenderWidth, DefaultRenderHeight);
}

Mario::Mario(float x, float y, const Sprite* sprite, int renderWidth, int renderHeight)
    : GameObject(x, y, 0.0f, 0.0f)
{
    SetCollidable(false);
    SetSprite(sprite);
    SetRenderSize(renderWidth, renderHeight);
}

void Mario::SetSprite(const Sprite* newSprite)
{
    sprite = newSprite;
}

void Mario::SetRenderSize(int newRenderWidth, int newRenderHeight)
{
    renderWidth = newRenderWidth > 0 ? newRenderWidth : 0;
    renderHeight = newRenderHeight > 0 ? newRenderHeight : 0;
    SetSize(static_cast<float>(renderWidth), static_cast<float>(renderHeight));
}

void Mario::Render(Renderer& renderer)
{
    if (!IsVisible() || !sprite || renderWidth <= 0 || renderHeight <= 0)
    {
        return;
    }

    renderer.DrawSprite(*sprite, static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y)), renderWidth, renderHeight);
}
