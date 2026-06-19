#include "Renderer.h"

void Renderer::Begin(HWND hwnd, COLORREF clearColor)
{
    windowHandle = hwnd;
    screenContext = GetDC(windowHandle);
    GetClientRect(windowHandle, &clientRect);

    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;

    backBufferContext = CreateCompatibleDC(screenContext);
    backBufferBitmap = CreateCompatibleBitmap(screenContext, width, height);
    oldBackBufferBitmap = SelectObject(backBufferContext, backBufferBitmap);

    HBRUSH brush = CreateSolidBrush(clearColor);
    FillRect(backBufferContext, &clientRect, brush);
    DeleteObject(brush);
}

void Renderer::DrawCenteredText(const std::wstring& text, int y, int height, int fontSize, COLORREF color)
{
    HFONT font = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Consolas");
    HGDIOBJ oldFont = SelectObject(backBufferContext, font);

    RECT textRect{ clientRect.left, y, clientRect.right, y + height };
    SetBkMode(backBufferContext, TRANSPARENT);
    SetTextColor(backBufferContext, color);
    DrawTextW(backBufferContext, text.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(backBufferContext, oldFont);
    DeleteObject(font);
}

void Renderer::DrawTextLine(const std::wstring& text, int x, int y, int fontSize, COLORREF color)
{
    HFONT font = CreateFontW(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Consolas");
    HGDIOBJ oldFont = SelectObject(backBufferContext, font);

    SetBkMode(backBufferContext, TRANSPARENT);
    SetTextColor(backBufferContext, color);
    TextOutW(backBufferContext, x, y, text.c_str(), static_cast<int>(text.size()));

    SelectObject(backBufferContext, oldFont);
    DeleteObject(font);
}

void Renderer::End()
{
    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;
    BitBlt(screenContext, 0, 0, width, height, backBufferContext, 0, 0, SRCCOPY);

    SelectObject(backBufferContext, oldBackBufferBitmap);
    DeleteObject(backBufferBitmap);
    DeleteDC(backBufferContext);
    ReleaseDC(windowHandle, screenContext);

    oldBackBufferBitmap = nullptr;
    backBufferBitmap = nullptr;
    backBufferContext = nullptr;
    screenContext = nullptr;
    windowHandle = nullptr;
}
