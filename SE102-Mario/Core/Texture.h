#pragma once

#include <string>
#include <windows.h>

class Texture
{
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    bool LoadFromFile(const std::wstring& filePath);
    void Unload();

    HBITMAP GetHandle() const;
    int GetWidth() const;
    int GetHeight() const;
    COLORREF GetPixelColor(int x, int y) const;

private:
    HBITMAP bitmap = nullptr;
    int width = 0;
    int height = 0;
    std::wstring sourcePath;
};
