#pragma once

#include <string>
#include <windows.h>

class Texture;

class Sprite
{
public:
    Sprite() = default;
    Sprite(std::string id, const Texture* texture, const RECT& sourceRect, bool useTransparentColor, COLORREF transparentColor);

    const std::string& GetId() const;
    const Texture* GetTexture() const;
    const RECT& GetSourceRect() const;
    int GetWidth() const;
    int GetHeight() const;
    bool UsesTransparentColor() const;
    COLORREF GetTransparentColor() const;

private:
    std::string id;
    const Texture* texture = nullptr;
    RECT sourceRect{};
    bool useTransparentColor = true;
    COLORREF transparentColor = RGB(255, 0, 255);
};
