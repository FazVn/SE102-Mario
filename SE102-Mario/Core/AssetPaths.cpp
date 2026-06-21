#include "AssetPaths.h"

#include <windows.h>

#include <algorithm>
#include <filesystem>
#include <system_error>
#include <vector>

namespace
{
    std::filesystem::path NormalizePath(std::wstring path)
    {
        std::replace(path.begin(), path.end(), L'/', L'\\');
        return std::filesystem::path(path);
    }

    void AddAncestors(std::vector<std::filesystem::path>& roots, std::filesystem::path start)
    {
        std::error_code errorCode;
        start = std::filesystem::absolute(start, errorCode);
        if (errorCode)
        {
            return;
        }

        while (!start.empty())
        {
            if (std::find(roots.begin(), roots.end(), start) == roots.end())
            {
                roots.push_back(start);
            }

            const std::filesystem::path parent = start.parent_path();
            if (parent == start)
            {
                break;
            }

            start = parent;
        }
    }

    std::filesystem::path GetExecutableDirectory()
    {
        wchar_t modulePath[MAX_PATH]{};
        const DWORD length = GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
        if (length == 0)
        {
            return std::filesystem::current_path();
        }

        return std::filesystem::path(modulePath).parent_path();
    }

    bool Exists(const std::filesystem::path& path)
    {
        std::error_code errorCode;
        return std::filesystem::exists(path, errorCode);
    }
}

namespace AssetPaths
{
    std::wstring ResolveResourcePath(const std::wstring& resourceRelativePath)
    {
        const std::filesystem::path relativePath = NormalizePath(resourceRelativePath);
        if (relativePath.is_absolute())
        {
            return relativePath.wstring();
        }

        std::vector<std::filesystem::path> roots;
        AddAncestors(roots, std::filesystem::current_path());
        AddAncestors(roots, GetExecutableDirectory());

        for (const std::filesystem::path& root : roots)
        {
            const std::filesystem::path direct = root / relativePath;
            if (Exists(direct))
            {
                return direct.wstring();
            }

            const std::filesystem::path resourceChild = root / L"Resources" / relativePath;
            if (Exists(resourceChild))
            {
                return resourceChild.wstring();
            }

            const std::filesystem::path projectResourceChild = root / L"SE102-Mario" / L"Resources" / relativePath;
            if (Exists(projectResourceChild))
            {
                return projectResourceChild.wstring();
            }
        }

        return (std::filesystem::current_path() / L"Resources" / relativePath).wstring();
    }

    std::wstring ToWide(const std::string& value)
    {
        if (value.empty())
        {
            return std::wstring();
        }

        const int length = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
        if (length <= 0)
        {
            return std::wstring(value.begin(), value.end());
        }

        std::wstring result(static_cast<size_t>(length), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, result.data(), length);
        if (!result.empty() && result.back() == L'\0')
        {
            result.pop_back();
        }

        return result;
    }
}
