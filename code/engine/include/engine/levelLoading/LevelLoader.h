#pragma once
#include <vector>
#include "CustomSerialization.h"
#include "engine/Assets.h"
#include "Objects.h"

namespace gl3::engine::levelLoading
{
    class LevelLoader
    {
    public:
        static void loadAllMetaData(
            const std::filesystem::path& levelDir = resolveAssetPath("levels"));
        static const std::vector<LevelMeta>& getMetaData() { return meta_data_; }
        static Level* loadLevelByID(int ID);

        static const std::unordered_map<int, std::unique_ptr<Level>>& getLevels()
        {
            return loaded_levels_;
        }

    private:
        static std::vector<LevelMeta> meta_data_;
        static std::unordered_map<int, std::unique_ptr<Level>> loaded_levels_; //Levels with IDs
        static std::unordered_map<int, std::string> idToFilename_;
        static Level* loadLevel(int ID, const std::string& filename);
    };
}

