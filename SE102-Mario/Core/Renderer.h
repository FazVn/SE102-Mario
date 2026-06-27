#pragma once

#include "RenderOptions.h"

#include <string>
#include <windows.h>

class Sprite;
class Texture;

class Renderer
{
public:
    void Begin(HWND windowHandle, COLORREF clearColor);
    void DrawTexture(const Texture& texture, int x, int y, const RECT& sourceRect, const RenderOptions& options = RenderOptions());
    void DrawTexture(const Texture& texture, int x, int y, int destinationWidth, int destinationHeight, const RECT& sourceRect, const RenderOptions& options = RenderOptions());
    void DrawTexture(const Texture& texture, int x, int y, const RECT& sourceRect, bool useTransparentColor, COLORREF transparentColor, const RenderOptions& options = RenderOptions());
    void DrawTexture(const Texture& texture, int x, int y, int destinationWidth, int destinationHeight, const RECT& sourceRect, bool useTransparentColor, COLORREF transparentColor, const RenderOptions& options = RenderOptions());
    void DrawSprite(const Texture& texture, int x, int y, int xLeft, int yTop, int xRight, int yDown, const RenderOptions& options = RenderOptions());
    void DrawSprite(const Texture& texture, int x, int y, int destinationWidth, int destinationHeight, int xLeft, int yTop, int xRight, int yDown, const RenderOptions& options = RenderOptions());
    void DrawSprite(const Sprite& sprite, int x, int y, const RenderOptions& options = RenderOptions());
    void DrawSprite(const Sprite& sprite, int x, int y, int destinationWidth, int destinationHeight, const RenderOptions& options = RenderOptions());
    void DrawFilledRect(int x, int y, int width, int height, COLORREF color);
    void DrawCenteredText(const std::wstring& text, int y, int height, int fontSize, COLORREF color, const wchar_t* fontFamily = L"Consolas", int fontWeight = FW_BOLD);
    void DrawTextLine(const std::wstring& text, int x, int y, int fontSize, COLORREF color, const wchar_t* fontFamily = L"Consolas", int fontWeight = FW_NORMAL);
    void End();

private:
    HWND windowHandle = nullptr;
    HDC screenContext = nullptr;
    HDC backBufferContext = nullptr;
    HBITMAP backBufferBitmap = nullptr;
    HGDIOBJ oldBackBufferBitmap = nullptr;
    RECT clientRect{};
};
