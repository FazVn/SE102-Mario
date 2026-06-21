#pragma once

#include <string>

namespace AssetPaths
{
    std::wstring ResolveResourcePath(const std::wstring& resourceRelativePath);
    std::wstring ToWide(const std::string& value);
}
