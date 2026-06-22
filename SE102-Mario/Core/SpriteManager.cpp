#include "SpriteManager.h"

#include "AssetPaths.h"
#include "Texture.h"
#include "TextureManager.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
    std::string RemoveComment(const std::string& line)
    {
        const size_t commentStart = line.find('#');
        return commentStart == std::string::npos ? line : line.substr(0, commentStart);
    }

    std::string ToLower(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character)
            {
                return static_cast<char>(std::tolower(character));
            });
        return value;
    }

    bool TryParseRgbTriplet(std::string token, COLORREF& color)
    {
        std::replace(token.begin(), token.end(), ',', ' ');
        std::istringstream parser(token);

        int red = 0;
        int green = 0;
        int blue = 0;
        if (!(parser >> red >> green >> blue))
        {
            return false;
        }

        color = RGB(red, green, blue);
        return true;
    }

    bool TryParseHexColor(const std::string& token, COLORREF& color)
    {
        if (token.rfind("0x", 0) != 0 && token.rfind("0X", 0) != 0)
        {
            return false;
        }

        try
        {
            const unsigned long value = std::stoul(token.substr(2), nullptr, 16);
            color = RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool ResolveTransparentColor(const std::string& token, const Texture& texture, const RECT& sourceRect, bool& useTransparentColor, COLORREF& transparentColor)
    {
        const std::string lowerToken = ToLower(token);
        if (lowerToken == "none")
        {
            useTransparentColor = false;
            transparentColor = RGB(255, 0, 255);
            return true;
        }

        useTransparentColor = true;
        if (lowerToken == "auto")
        {
            transparentColor = texture.GetPixelColor(sourceRect.left, sourceRect.top);
            return true;
        }

        return TryParseHexColor(token, transparentColor) || TryParseRgbTriplet(token, transparentColor);
    }
}

bool SpriteManager::Add(const std::string& id, const Sprite& sprite)
{
    sprites[id] = sprite;
    return true;
}

bool SpriteManager::Add(const std::string& id, const Texture* texture, int xLeft, int yTop, int xRight, int yDown, bool useTransparentColor, COLORREF transparentColor)
{
    if (!texture || xRight <= xLeft || yDown <= yTop)
    {
        return false;
    }

    RECT sourceRect{ xLeft, yTop, xRight, yDown };
    return Add(id, Sprite(id, texture, sourceRect, useTransparentColor, transparentColor));
}

bool SpriteManager::LoadFromDefinitionFile(const std::wstring& resourceDefinitionPath, const TextureManager& textureManager)
{
    const std::wstring resolvedPath = AssetPaths::ResolveResourcePath(resourceDefinitionPath);
    std::ifstream input{ std::filesystem::path(resolvedPath) };
    if (!input)
    {
        return false;
    }

    bool allLoaded = true;
    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream parser(RemoveComment(line));

        std::string id;
        std::string textureId;
        int xLeft = 0;
        int yTop = 0;
        int xRight = 0;
        int yDown = 0;
        std::string transparentToken = "auto";

        if (!(parser >> id >> textureId >> xLeft >> yTop >> xRight >> yDown))
        {
            continue;
        }

        parser >> transparentToken;

        const Texture* texture = textureManager.Get(textureId);
        if (!texture || xRight <= xLeft || yDown <= yTop)
        {
            allLoaded = false;
            continue;
        }

        RECT sourceRect{ xLeft, yTop, xRight, yDown };
        bool useTransparentColor = true;
        COLORREF transparentColor = RGB(255, 0, 255);
        if (!ResolveTransparentColor(transparentToken, *texture, sourceRect, useTransparentColor, transparentColor))
        {
            allLoaded = false;
            continue;
        }

        Add(id, texture, xLeft, yTop, xRight, yDown, useTransparentColor, transparentColor);
    }

    return allLoaded;
}

const Sprite* SpriteManager::Get(const std::string& id) const
{
    const auto iterator = sprites.find(id);
    return iterator == sprites.end() ? nullptr : &iterator->second;
}

void SpriteManager::Clear()
{
    sprites.clear();
}
