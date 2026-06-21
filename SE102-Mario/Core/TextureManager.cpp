#include "TextureManager.h"

#include "AssetPaths.h"

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
}

bool TextureManager::Load(const std::string& id, const std::wstring& resourcePath)
{
    auto texture = std::make_unique<Texture>();
    const std::wstring resolvedPath = AssetPaths::ResolveResourcePath(resourcePath);
    if (!texture->LoadFromFile(resolvedPath))
    {
        return false;
    }

    textures[id] = std::move(texture);
    return true;
}

bool TextureManager::LoadFromDefinitionFile(const std::wstring& resourceDefinitionPath)
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
        std::string path;
        if (!(parser >> id >> path))
        {
            continue;
        }

        if (!Load(id, AssetPaths::ToWide(path)))
        {
            allLoaded = false;
        }
    }

    return allLoaded;
}

const Texture* TextureManager::Get(const std::string& id) const
{
    const auto iterator = textures.find(id);
    return iterator == textures.end() ? nullptr : iterator->second.get();
}

void TextureManager::Clear()
{
    textures.clear();
}
