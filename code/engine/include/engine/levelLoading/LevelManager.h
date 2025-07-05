#pragma once
#include <vector>
#include "CustomSerialization.h"
#include "engine/Assets.h"
#include "Objects.h"

namespace gl3::engine::levelLoading
{
    class LevelManager
    {
    public:
        static void loadAllMetaData(
            const std::filesystem::path& levelDir = resolveAssetPath("levels"));
        static const std::vector<LevelMeta>& getMetaData() { return meta_data_; }
        static Level* loadLevelByID(int ID);
        static void addObjectToCurrentLevel(const GameObject& object);
        static void removeObjectAtPosition(glm::vec2 position);
        static void saveCurrentLevel();

        static const std::unordered_map<int, std::unique_ptr<Level>>& getLevels()
        {
            return loaded_levels_;
        }

    private:
        static std::vector<LevelMeta> meta_data_;
        static std::unordered_map<int, std::unique_ptr<Level>> loaded_levels_; //Levels with IDs
        static std::unordered_map<int, std::string> idToFilename_;
        static Level* loadLevel(int ID, const std::string& filename);
        static int most_recent_loaded_lvl_ID;
    };
}

