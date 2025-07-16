#include "engine/levelloading/LevelManager.h"
#include <filesystem>
#include <glaze/json/read.hpp>

namespace gl3::engine::levelLoading
{
    int LevelManager::most_recent_loaded_lvl_ID = 0;
    std::vector<LevelMeta> LevelManager::meta_data_;
    std::unordered_map<int, std::unique_ptr<Level>> LevelManager::loaded_levels_;
    std::unordered_map<int, std::string> LevelManager::idToFilename_;

    namespace fs = std::filesystem;

    /**
* Load a single level from a json file in assets/levles, if the level is already loaded, just return it.
*/
    Level* LevelManager::loadLevel(const int ID, const std::string& filename)
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
        most_recent_loaded_lvl_ID = it->first;
        return it->second.get();
    }

    /**
* Resolves the levelID to a path and returns a pointer to the level.
*/
    Level* LevelManager::loadLevelByID(const int ID)
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
    void LevelManager::loadAllMetaData(
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

    void LevelManager::addObjectToCurrentLevel(const GameObject& object)
    {
        auto it = loaded_levels_.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels_.end() || !it->second)
        {
            throw std::runtime_error("No current level loaded to add object.");
        }

        Level* level = it->second.get();
        level->objects.push_back(object);
    }

    void LevelManager::removeAllObjectsAtPosition(glm::vec2 position)
    {
        auto it = loaded_levels_.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels_.end() || !it->second)
        {
            throw std::runtime_error("No current level loaded to remove object.");
        }

        Level* level = it->second.get();

        // Remove all objects whose positions match
        auto& objects = level->objects;
        std::erase_if(objects,
                      [&](const GameObject& obj)
                      {
                          return obj.position.x == position.x && obj.position.y == position.y && obj.tag != "background"
                              && obj.tag != "ground" && obj.tag != "sky";
                      });
        std::vector<std::string> groupsToRemove;

        for (auto& group : level->groups)
        {
            auto& groupObjs = group.children;
            std::erase_if(groupObjs, [&](const GameObject& obj)
            {
                return obj.position.x == position.x && obj.position.y == position.y;
            });

            if (group.children.empty())
            {
                groupsToRemove.push_back(group.name);
            }
        }

        // erase emptied groups
        for (const auto& name : groupsToRemove)
        {
            removeGroupByName(name);
        }
    }

    void LevelManager::addGroupToCurrentLevel(const GameObjectGroup& group)
    {
        auto it = loaded_levels_.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels_.end() || !it->second)
        {
            throw std::runtime_error("No current level loaded to add group.");
        }

        Level* level = it->second.get();
        level->groups.push_back(group);
    }

    void LevelManager::removeGroupByName(const std::string& groupName)
    {
        auto it = loaded_levels_.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels_.end() || !it->second)
        {
            throw std::runtime_error("No current level loaded to remove group.");
        }

        Level* level = it->second.get();
        auto& groups = level->groups;

        std::erase_if(groups, [&](const GameObjectGroup& group)
        {
            return group.name == groupName;
        });
    }


    void LevelManager::saveCurrentLevel()
    {
        auto it = loaded_levels_.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels_.end())
        {
            throw std::runtime_error("Level with ID " + std::to_string(most_recent_loaded_lvl_ID) + " is not loaded.");
        }

        Level* level = it->second.get();

        auto result = glz::write_json(*level); // returns expected<string, error_ctx>
        if (!result)
        {
            throw std::runtime_error("Failed to serialize level: " + std::to_string(most_recent_loaded_lvl_ID));
        }

        auto filenameIt = idToFilename_.find(most_recent_loaded_lvl_ID);
        if (filenameIt == idToFilename_.end())
        {
            throw std::runtime_error("No filename mapped for level ID " + std::to_string(most_recent_loaded_lvl_ID));
        }

        const auto path = std::filesystem::path(resolveAssetPath("levels")) / filenameIt->second;

        std::ofstream file(path);
        if (!file)
        {
            throw std::runtime_error("Failed to open file for writing: " + path.string());
        }

        file << *result; // Dereference expected to get the string
        file.close();
    }
}
