#include "Texture.h"

#include <gdiplus.h>
#include <utility>

#pragma comment(lib, "gdiplus.lib")

namespace
{
    class GdiPlusSession
    {
    public:
        GdiPlusSession()
        {
            Gdiplus::GdiplusStartupInput startupInput;
            ready = Gdiplus::GdiplusStartup(&token, &startupInput, nullptr) == Gdiplus::Ok;
        }

        ~GdiPlusSession()
        {
            if (ready)
            {
                Gdiplus::GdiplusShutdown(token);
            }
        }

        bool IsReady() const
        {
            return ready;
        }

    private:
        ULONG_PTR token = 0;
        bool ready = false;
    };

    GdiPlusSession& GetGdiPlusSession()
    {
        static GdiPlusSession session;
        return session;
    }
}

Texture::~Texture()
{
    Unload();
}

Texture::Texture(Texture&& other) noexcept
{
    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        Unload();

        bitmap = other.bitmap;
        width = other.width;
        height = other.height;
        sourcePath = std::move(other.sourcePath);

        other.bitmap = nullptr;
        other.width = 0;
        other.height = 0;
    }

    return *this;
}

bool Texture::LoadFromFile(const std::wstring& filePath)
{
    if (!GetGdiPlusSession().IsReady())
    {
        return false;
    }

    Gdiplus::Bitmap source(filePath.c_str());
    if (source.GetLastStatus() != Gdiplus::Ok)
    {
        return false;
    }

    HBITMAP loadedBitmap = nullptr;
    const Gdiplus::Color transparentBackground(0, 0, 0, 0);
    if (source.GetHBITMAP(transparentBackground, &loadedBitmap) != Gdiplus::Ok || !loadedBitmap)
    {
        return false;
    }

    Unload();

    bitmap = loadedBitmap;
    width = static_cast<int>(source.GetWidth());
    height = static_cast<int>(source.GetHeight());
    sourcePath = filePath;

    return true;
}

void Texture::Unload()
{
    if (bitmap)
    {
        DeleteObject(bitmap);
        bitmap = nullptr;
    }

    width = 0;
    height = 0;
    sourcePath.clear();
}

HBITMAP Texture::GetHandle() const
{
    return bitmap;
}

int Texture::GetWidth() const
{
    return width;
}

int Texture::GetHeight() const
{
    return height;
}

COLORREF Texture::GetPixelColor(int x, int y) const
{
    if (!bitmap || x < 0 || y < 0 || x >= width || y >= height)
    {
        return RGB(255, 0, 255);
    }

    HDC screenContext = GetDC(nullptr);
    HDC bitmapContext = CreateCompatibleDC(screenContext);
    HGDIOBJ oldBitmap = SelectObject(bitmapContext, bitmap);

    COLORREF color = GetPixel(bitmapContext, x, y);
    if (color == CLR_INVALID)
    {
        color = RGB(255, 0, 255);
    }

    SelectObject(bitmapContext, oldBitmap);
    DeleteDC(bitmapContext);
    ReleaseDC(nullptr, screenContext);

    return color;
}
