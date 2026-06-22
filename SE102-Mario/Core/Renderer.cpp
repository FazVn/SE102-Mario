#include "Renderer.h"

#include "Sprite.h"
#include "Texture.h"

#include <algorithm>
#include <cmath>

#pragma comment(lib, "Msimg32.lib")

namespace
{
    HFONT CreateRenderFont(int fontSize, int fontWeight, const wchar_t* fontFamily)
    {
        return CreateFontW(fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
            fontFamily ? fontFamily : L"Consolas");
    }

    void FillBitmap(HDC targetContext, int width, int height, COLORREF color)
    {
        RECT rect{ 0, 0, width, height };
        HBRUSH brush = CreateSolidBrush(color);
        FillRect(targetContext, &rect, brush);
        DeleteObject(brush);
    }

    RECT MakeSourceRect(int xLeft, int yTop, int xRight, int yDown)
    {
        return RECT{ xLeft, yTop, xRight, yDown };
    }
}

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

void Renderer::DrawTexture(const Texture& texture, int x, int y, const RECT& sourceRect, const RenderOptions& options)
{
    DrawTexture(texture, x, y, sourceRect, false, RGB(255, 0, 255), options);
}

void Renderer::DrawTexture(const Texture& texture, int x, int y, int destinationWidth, int destinationHeight, const RECT& sourceRect, const RenderOptions& options)
{
    DrawTexture(texture, x, y, destinationWidth, destinationHeight, sourceRect, false, RGB(255, 0, 255), options);
}

void Renderer::DrawTexture(const Texture& texture, int x, int y, const RECT& sourceRect, bool useTransparentColor, COLORREF transparentColor, const RenderOptions& options)
{
    const int sourceWidth = sourceRect.right - sourceRect.left;
    const int sourceHeight = sourceRect.bottom - sourceRect.top;
    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        return;
    }

    const float scale = options.scale <= 0.0f ? 1.0f : options.scale;
    const int scaledWidthCandidate = static_cast<int>(std::lround(sourceWidth * scale));
    const int scaledHeightCandidate = static_cast<int>(std::lround(sourceHeight * scale));
    const int scaledWidth = scaledWidthCandidate > 1 ? scaledWidthCandidate : 1;
    const int scaledHeight = scaledHeightCandidate > 1 ? scaledHeightCandidate : 1;
    DrawTexture(texture, x, y, scaledWidth, scaledHeight, sourceRect, useTransparentColor, transparentColor, options);
}

void Renderer::DrawTexture(const Texture& texture, int x, int y, int destinationWidth, int destinationHeight, const RECT& sourceRect, bool useTransparentColor, COLORREF transparentColor, const RenderOptions& options)
{
    if (!backBufferContext || !texture.GetHandle())
    {
        return;
    }

    const int sourceWidth = sourceRect.right - sourceRect.left;
    const int sourceHeight = sourceRect.bottom - sourceRect.top;
    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        return;
    }

    if (destinationWidth <= 0 || destinationHeight <= 0)
    {
        return;
    }

    HDC textureContext = CreateCompatibleDC(backBufferContext);
    HGDIOBJ oldTextureBitmap = SelectObject(textureContext, texture.GetHandle());

    SetStretchBltMode(backBufferContext, COLORONCOLOR);
    SetStretchBltMode(textureContext, COLORONCOLOR);

    const bool hasFlip = options.flipX || options.flipY;
    const BYTE alpha = static_cast<BYTE>(std::clamp(options.alpha, 0.0f, 1.0f) * 255.0f);

    if (useTransparentColor)
    {
        if (!hasFlip && alpha == 255)
        {
            TransparentBlt(backBufferContext, x, y, destinationWidth, destinationHeight,
                textureContext, sourceRect.left, sourceRect.top, sourceWidth, sourceHeight, transparentColor);
        }
        else
        {
            HDC tempContext = CreateCompatibleDC(backBufferContext);
            HBITMAP tempBitmap = CreateCompatibleBitmap(backBufferContext, destinationWidth, destinationHeight);
            HGDIOBJ oldTempBitmap = SelectObject(tempContext, tempBitmap);
            FillBitmap(tempContext, destinationWidth, destinationHeight, transparentColor);

            TransparentBlt(tempContext, 0, 0, destinationWidth, destinationHeight,
                textureContext, sourceRect.left, sourceRect.top, sourceWidth, sourceHeight, transparentColor);

            HDC drawContext = tempContext;
            HBITMAP mirroredBitmap = nullptr;
            HGDIOBJ oldMirroredBitmap = nullptr;
            HDC mirroredContext = nullptr;

            if (hasFlip)
            {
                mirroredContext = CreateCompatibleDC(backBufferContext);
                mirroredBitmap = CreateCompatibleBitmap(backBufferContext, destinationWidth, destinationHeight);
                oldMirroredBitmap = SelectObject(mirroredContext, mirroredBitmap);
                FillBitmap(mirroredContext, destinationWidth, destinationHeight, transparentColor);

                const int drawX = options.flipX ? destinationWidth : 0;
                const int drawY = options.flipY ? destinationHeight : 0;
                const int drawWidth = options.flipX ? -destinationWidth : destinationWidth;
                const int drawHeight = options.flipY ? -destinationHeight : destinationHeight;
                StretchBlt(mirroredContext, drawX, drawY, drawWidth, drawHeight,
                    tempContext, 0, 0, destinationWidth, destinationHeight, SRCCOPY);

                drawContext = mirroredContext;
            }

            TransparentBlt(backBufferContext, x, y, destinationWidth, destinationHeight,
                drawContext, 0, 0, destinationWidth, destinationHeight, transparentColor);

            if (mirroredContext)
            {
                SelectObject(mirroredContext, oldMirroredBitmap);
                DeleteObject(mirroredBitmap);
                DeleteDC(mirroredContext);
            }

            SelectObject(tempContext, oldTempBitmap);
            DeleteObject(tempBitmap);
            DeleteDC(tempContext);
        }
    }
    else
    {
        const int drawX = options.flipX ? x + destinationWidth : x;
        const int drawY = options.flipY ? y + destinationHeight : y;
        const int drawWidth = options.flipX ? -destinationWidth : destinationWidth;
        const int drawHeight = options.flipY ? -destinationHeight : destinationHeight;

        if (!hasFlip && alpha < 255)
        {
            BLENDFUNCTION blend{};
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = alpha;

            AlphaBlend(backBufferContext, x, y, destinationWidth, destinationHeight,
                textureContext, sourceRect.left, sourceRect.top, sourceWidth, sourceHeight, blend);
        }
        else
        {
            StretchBlt(backBufferContext, drawX, drawY, drawWidth, drawHeight,
                textureContext, sourceRect.left, sourceRect.top, sourceWidth, sourceHeight, SRCCOPY);
        }
    }

    SelectObject(textureContext, oldTextureBitmap);
    DeleteDC(textureContext);
}

void Renderer::DrawSprite(const Texture& texture, int x, int y, int xLeft, int yTop, int xRight, int yDown, const RenderOptions& options)
{
    DrawTexture(texture, x, y, MakeSourceRect(xLeft, yTop, xRight, yDown), options);
}

void Renderer::DrawSprite(const Texture& texture, int x, int y, int destinationWidth, int destinationHeight, int xLeft, int yTop, int xRight, int yDown, const RenderOptions& options)
{
    DrawTexture(texture, x, y, destinationWidth, destinationHeight, MakeSourceRect(xLeft, yTop, xRight, yDown), options);
}

void Renderer::DrawSprite(const Sprite& sprite, int x, int y, const RenderOptions& options)
{
    const Texture* texture = sprite.GetTexture();
    if (!texture)
    {
        return;
    }

    DrawTexture(*texture, x, y, sprite.GetSourceRect(), sprite.UsesTransparentColor(), sprite.GetTransparentColor(), options);
}

void Renderer::DrawSprite(const Sprite& sprite, int x, int y, int destinationWidth, int destinationHeight, const RenderOptions& options)
{
    const Texture* texture = sprite.GetTexture();
    if (!texture)
    {
        return;
    }

    DrawTexture(*texture, x, y, destinationWidth, destinationHeight, sprite.GetSourceRect(), sprite.UsesTransparentColor(), sprite.GetTransparentColor(), options);
}

void Renderer::DrawCenteredText(const std::wstring& text, int y, int height, int fontSize, COLORREF color, const wchar_t* fontFamily, int fontWeight)
{
    HFONT font = CreateRenderFont(fontSize, fontWeight, fontFamily);
    HGDIOBJ oldFont = SelectObject(backBufferContext, font);

    RECT textRect{ clientRect.left, y, clientRect.right, y + height };
    SetBkMode(backBufferContext, TRANSPARENT);
    SetTextColor(backBufferContext, color);
    DrawTextW(backBufferContext, text.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(backBufferContext, oldFont);
    DeleteObject(font);
}

void Renderer::DrawTextLine(const std::wstring& text, int x, int y, int fontSize, COLORREF color, const wchar_t* fontFamily, int fontWeight)
{
    HFONT font = CreateRenderFont(fontSize, fontWeight, fontFamily);
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
