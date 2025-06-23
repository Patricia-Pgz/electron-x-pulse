#include "engine/levelloading/LevelLoader.h"
#include <filesystem>
#include <glaze/json/read.hpp>

namespace gl3::engine::levelLoading
{
    std::vector<LevelMeta> LevelLoader::meta_data_;
    std::unordered_map<int, std::unique_ptr<Level>> LevelLoader::loaded_levels_;
    std::unordered_map<int, std::string> LevelLoader::idToFilename_;

    namespace fs = std::filesystem;

    /**
* Load a single level from a json file in assets/levles, if the level is already loaded, just return it.
*/
    Level* LevelLoader::loadLevel(const int ID, const std::string& filename)
    {
        if (const auto existingLevel = loaded_levels_.find(ID); existingLevel != loaded_levels_.end())
        {
            return existingLevel->second.get();
        }

        const auto path = std::filesystem::path(resolveAssetPath("levels")) / filename;
        std::ifstream file(path);
        if (!file)
        {
            throw std::runtime_error("Failed to open level file: " + path.string());
        }

        std::string json((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());

        auto level = std::make_unique<Level>();
        if (const auto err = glz::read_json(*level, json); err)
        {
            throw std::runtime_error("Failed to parse level JSON: " + std::to_string(static_cast<float>(err.ec)));
        }

        auto [it, _] = loaded_levels_.emplace(ID, std::move(level));
        return it->second.get();
    }

    /**
* Resolves the levelID to a path and returns a pointer to the level.
*/
    Level* LevelLoader::loadLevelByID(const int ID)
    {
        const auto levelFileName = idToFilename_.find(ID);
        if (levelFileName == idToFilename_.end())
        {
            throw std::runtime_error("No level filename for id " + std::to_string(ID));
        }

        return loadLevel(levelFileName->first, levelFileName->second);
    }

    /**
    * Load metadata for all levels from all meda data files in assets/levles folder
    */
    void LevelLoader::loadAllMetaData(
        const std::filesystem::path& levelDir)
    {
        for (const auto& entry : std::filesystem::directory_iterator(levelDir))
        {
            if (!entry.is_regular_file()) continue;

            const auto& path = entry.path();
            if (path.extension() != ".json" || path.filename().string().find(".meta") == std::string::npos)
                continue;

            try
            {
                std::ifstream file(path);
                if (!file) throw std::runtime_error("Cannot open file");

                std::string json((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());

                LevelMeta meta;
                if (const auto err = glz::read_json(meta, json); err)
                {
                    std::cerr << err;
                }

                idToFilename_[meta.id] = meta.fileName;
                meta_data_.emplace_back(std::move(meta));
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error reading " << path << ": " << e.what() << '\n';
            }
        }
    }
}
