#pragma once

#include <string>
#include <windows.h>

class Renderer
{
public:
    void Begin(HWND windowHandle, COLORREF clearColor);
    void DrawCenteredText(const std::wstring& text, int y, int height, int fontSize, COLORREF color);
    void DrawTextLine(const std::wstring& text, int x, int y, int fontSize, COLORREF color);
    void End();

private:
    HWND windowHandle = nullptr;
    HDC screenContext = nullptr;
    HDC backBufferContext = nullptr;
    HBITMAP backBufferBitmap = nullptr;
    HGDIOBJ oldBackBufferBitmap = nullptr;
    RECT clientRect{};
};
