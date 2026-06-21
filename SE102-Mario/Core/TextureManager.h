#pragma once

#include "Texture.h"

#include <memory>
#include <string>
#include <unordered_map>

class TextureManager
{
public:
    bool Load(const std::string& id, const std::wstring& resourcePath);
    bool LoadFromDefinitionFile(const std::wstring& resourceDefinitionPath);
    const Texture* Get(const std::string& id) const;
    void Clear();

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};
