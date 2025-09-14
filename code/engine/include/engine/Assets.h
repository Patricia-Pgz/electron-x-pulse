#pragma once
#include <filesystem>
#include <Windows.h>
#include <stdexcept>

#define GET_STRING(x) #x
#define GET_DIR(x) GET_STRING(x)

namespace fs = std::filesystem;

namespace gl3::engine
{
    /**
     * @brief Gets the full path to the current executable.
     */
    inline fs::path getExecutablePath()
    {
        char buffer[MAX_PATH];
        if (const DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH); length == 0 || length == MAX_PATH)
        {
            throw std::runtime_error("Failed to get executable path.");
        }
        return buffer;
    }

    /**
     * @brief Resolves the absolute path for an asset.
     *
     * This utility joins the `ASSET_ROOT` macro with a relative asset path
     * and returns the canonical (absolute, normalized) path as a string.
     *
     * @param relativeAssetPath The relative path to the asset within the asset directory.
     * @return The canonical absolute path to the asset as a string.
     */
    inline std::string resolveAssetPath(const fs::path& relativeAssetPath)
    {
        auto exeDir = getExecutablePath().parent_path();
        auto mergedPath = (exeDir / ASSET_ROOT / relativeAssetPath).make_preferred();
        return canonical(mergedPath).string();
    }
}
