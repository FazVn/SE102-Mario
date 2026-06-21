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
    void DrawTexture(const Texture& texture, int x, int y, const RECT& sourceRect, bool useTransparentColor, COLORREF transparentColor, const RenderOptions& options = RenderOptions());
    void DrawSprite(const Sprite& sprite, int x, int y, const RenderOptions& options = RenderOptions());
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
