#include "Sprite.h"

#include <utility>

Sprite::Sprite(std::string id, const Texture* texture, const RECT& sourceRect, bool useTransparentColor, COLORREF transparentColor)
    : id(std::move(id)),
    texture(texture),
    sourceRect(sourceRect),
    useTransparentColor(useTransparentColor),
    transparentColor(transparentColor)
{
}

const std::string& Sprite::GetId() const
{
    return id;
}

const Texture* Sprite::GetTexture() const
{
    return texture;
}

const RECT& Sprite::GetSourceRect() const
{
    return sourceRect;
}

int Sprite::GetWidth() const
{
    return sourceRect.right - sourceRect.left;
}

int Sprite::GetHeight() const
{
    return sourceRect.bottom - sourceRect.top;
}

bool Sprite::UsesTransparentColor() const
{
    return useTransparentColor;
}

COLORREF Sprite::GetTransparentColor() const
{
    return transparentColor;
}
