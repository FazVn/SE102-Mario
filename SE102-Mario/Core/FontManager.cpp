#include "FontManager.h"

#include "AssetPaths.h"

FontManager::~FontManager()
{
    Clear();
}

bool FontManager::Load(const std::string& id, const std::wstring& resourcePath, const std::wstring& familyName)
{
    const auto existing = fonts.find(id);
    if (existing != fonts.end())
    {
        RemoveFontResourceExW(existing->second.filePath.c_str(), FR_PRIVATE, nullptr);
        fonts.erase(existing);
    }

    const std::wstring resolvedPath = AssetPaths::ResolveResourcePath(resourcePath);
    if (AddFontResourceExW(resolvedPath.c_str(), FR_PRIVATE, nullptr) == 0)
    {
        return false;
    }

    fonts[id] = FontInfo{ resolvedPath, familyName };
    return true;
}

const std::wstring* FontManager::GetFamily(const std::string& id) const
{
    const auto iterator = fonts.find(id);
    return iterator == fonts.end() ? nullptr : &iterator->second.familyName;
}

void FontManager::Clear()
{
    for (const auto& entry : fonts)
    {
        RemoveFontResourceExW(entry.second.filePath.c_str(), FR_PRIVATE, nullptr);
    }

    fonts.clear();
}
