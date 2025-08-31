#include "engine/levelloading/LevelManager.h"
#include <filesystem>
#include <iostream>
#include <glaze/json/read.hpp>

namespace gl3::engine::levelLoading
{
    int LevelManager::most_recent_loaded_lvl_ID = 0;
    std::vector<LevelMeta> LevelManager::meta_data;
    std::unordered_map<int, std::unique_ptr<Level>> LevelManager::loaded_levels;
    std::unordered_map<int, std::string> LevelManager::idToFilename;

    namespace fs = std::filesystem;

    // Load a single level from a json file in assets/levels, if the level is already loaded, just return it.

    Level* LevelManager::loadLevel(const int ID, const std::string& filename)
    {
        if (const auto existingLevel = loaded_levels.find(ID); existingLevel != loaded_levels.end())
        {
            most_recent_loaded_lvl_ID = existingLevel->first;
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

        auto [it, _] = loaded_levels.emplace(ID, std::move(level));
        most_recent_loaded_lvl_ID = it->first;
        return it->second.get();
    }

    // Resolves the levelID to a path and returns a pointer to the level.
    Level* LevelManager::loadLevelByID(const int ID)
    {
        const auto levelFileName = idToFilename.find(ID);
        if (levelFileName == idToFilename.end())
        {
            throw std::runtime_error("No level filename for id " + std::to_string(ID));
        }

        return loadLevel(levelFileName->first, levelFileName->second);
    }

    //Load metadata for all levels from all meda data files in assets/levels folder
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

                idToFilename[meta.id] = meta.fileName;
                meta_data.emplace_back(std::move(meta));
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error reading " << path << ": " << e.what() << '\n';
            }
        }
    }

    Level* LevelManager::getMostRecentLoadedLevel()
    {
        if (const auto it = loaded_levels.find(most_recent_loaded_lvl_ID); it != loaded_levels.end())
        {
            return it->second.get();
        }
        return nullptr; // not loaded
    }


    void LevelManager::addObjectToCurrentLevel(const GameObject& object)
    {
        const auto it = loaded_levels.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels.end() || !it->second)
        {
            throw std::runtime_error("No current level loaded to add object.");
        }

        Level* level = it->second.get();
        level->objects.push_back(object);
    }

    void LevelManager::removeAllObjectsAtPosition(glm::vec2 position)
    {
        const auto it = loaded_levels.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels.end() || !it->second)
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
        const auto it = loaded_levels.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels.end() || !it->second)
        {
            throw std::runtime_error("No current level loaded to add group.");
        }

        Level* level = it->second.get();
        level->groups.push_back(group);
    }

    void LevelManager::removeGroupByName(const std::string& groupName)
    {
        const auto it = loaded_levels.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels.end() || !it->second)
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

    void LevelManager::roundObjectData(GameObject& object)
    {
        object.position.x = std::round(object.position.x * 100.0f) / 100.0f;
        object.position.y = std::round(object.position.y * 100.0f) / 100.0f;
        object.position.z = std::round(object.position.z * 100.0f) / 100.0f;

        object.scale.x = std::round(object.scale.x * 100.0f) / 100.0f;
        object.scale.y = std::round(object.scale.y * 100.0f) / 100.0f;
        object.scale.z = std::round(object.scale.z * 100.0f) / 100.0f;
    }

    void LevelManager::saveCurrentLevel()
    {
        const auto it = loaded_levels.find(most_recent_loaded_lvl_ID);
        if (it == loaded_levels.end())
        {
            throw std::runtime_error("Level with ID " + std::to_string(most_recent_loaded_lvl_ID) + " is not loaded.");
        }

        Level* level = it->second.get();
        for (auto& element : level->backgrounds)
        {
            roundObjectData(element);
        }
        for (auto& group : level->groups)
        {
            for (auto& element : group.children)
            {
                roundObjectData(element);
            }
            roundObjectData(group.colliderAABB);
        }
        for (auto& element : level->objects)
        {
            roundObjectData(element);
        }

        const auto result = glz::write_json(*level); // returns expected<string, error_ctx>
        if (!result)
        {
            throw std::runtime_error("Failed to serialize level: " + std::to_string(most_recent_loaded_lvl_ID));
        }

        const auto filenameIt = idToFilename.find(most_recent_loaded_lvl_ID);
        if (filenameIt == idToFilename.end())
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
