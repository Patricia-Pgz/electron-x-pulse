#pragma once
#include <filesystem>

#define GET_STRING(x) #x
#define GET_DIR(x) GET_STRING(x)

namespace fs = std::filesystem;

namespace gl3::engine
{
    /**
     * @brief Resolves the absolute path for an asset.
     *
     * This utility joins the compile-time `ASSET_ROOT` macro with a relative asset path
     * and returns the canonical (absolute, normalized) path as a string.
     *
     * @param relativeAssetPath The relative path to the asset within the asset directory.
     * @return The canonical absolute path to the asset as a string.
     */
    inline std::string resolveAssetPath(const fs::path& relativeAssetPath)
    {
        auto mergedPath = (GET_DIR(ASSET_ROOT) / relativeAssetPath).make_preferred();
        return canonical(mergedPath).string();
    }
}
