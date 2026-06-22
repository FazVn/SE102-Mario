#pragma once

#include "Sprite.h"

#include <string>
#include <unordered_map>

class TextureManager;

class SpriteManager
{
public:
    bool Add(const std::string& id, const Sprite& sprite);
    bool Add(const std::string& id, const Texture* texture, int xLeft, int yTop, int xRight, int yDown, bool useTransparentColor, COLORREF transparentColor);
    bool LoadFromDefinitionFile(const std::wstring& resourceDefinitionPath, const TextureManager& textureManager);
    const Sprite* Get(const std::string& id) const;
    void Clear();

private:
    std::unordered_map<std::string, Sprite> sprites;
};
