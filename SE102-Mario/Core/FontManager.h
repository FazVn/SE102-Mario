#pragma once

#include <string>
#include <unordered_map>
#include <windows.h>

class FontManager
{
public:
    ~FontManager();

    bool Load(const std::string& id, const std::wstring& resourcePath, const std::wstring& familyName);
    const std::wstring* GetFamily(const std::string& id) const;
    void Clear();

private:
    struct FontInfo
    {
        std::wstring filePath;
        std::wstring familyName;
    };

    std::unordered_map<std::string, FontInfo> fonts;
};
