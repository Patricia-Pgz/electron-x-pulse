#pragma once
#include <vector>
#include "CustomSerialization.h"


#include "engine/Assets.h"

namespace gl3::engine::levelLoading
{
    class LevelLoader
    {
    public:
        static std::vector<LevelMeta> loadAllMetaData(const std::filesystem::path& levelDir = resolveAssetPath("levels"));
        static const std::vector<Level>& getLevels() { return levels; }

    private:
        static std::vector<Level> levels;
        static Level loadLevel(const std::filesystem::path& filePath);
    };
}

