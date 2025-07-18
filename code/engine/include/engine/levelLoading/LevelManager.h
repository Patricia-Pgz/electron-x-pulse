#pragma once
#include <vector>
#include "CustomSerialization.h"
#include "engine/Assets.h"
#include "Objects.h"

namespace gl3::engine::levelLoading
{
    /**
     * @class LevelManager
     * @brief Static manager for handling game level loading, saving, and editing.
     *
     * Provides functionality to load all level metadata, load and retrieve levels by ID,
     * modify the current level by adding/removing objects and groups, and save the current level.
     * Maintains maps of loaded levels and their metadata for efficient access.
     */
    class LevelManager
    {
    public:
       /**
        * @brief Loads metadata for all levels from the specified directory.
        *
        * This method scans the given directory for level metadata files and loads them into
        * the internal metadata cache.
        *
        * @param levelDir Directory path to load level metadata from. Defaults to "levels" asset path.
        */
        static void loadAllMetaData(const std::filesystem::path& levelDir = resolveAssetPath("levels"));

        /**
         * @brief Retrieves all loaded level metadata.
         *
         * @return Const reference to a vector containing metadata of all available levels.
         */
        static const std::vector<LevelMeta>& getMetaData() { return meta_data_; }

        /**
         * @brief Loads and returns a level by its ID.
         *
         * If the level is already loaded, returns the cached version; otherwise loads it from disk.
         *
         * @param ID Unique identifier of the level to load.
         * @return Pointer to the loaded Level object, or runtime error if not found or failed to load.
         */
        static Level* loadLevelByID(int ID);

        /**
         * @brief Adds a game object to the currently loaded level.
         *
         * @param object The GameObject to add.
         */
        static void addObjectToCurrentLevel(const GameObject& object);

        /**
         * @brief Removes all game objects located at the specified position from the current level.
         *
         * @param position Position to remove objects from.
         */
        static void removeAllObjectsAtPosition(glm::vec2 position);

        /**
         * @brief Adds a group of game objects to the currently loaded level.
         *
         * @param group The GameObjectGroup to add.
         */
        static void addGroupToCurrentLevel(const GameObjectGroup& group);

        /**
         * @brief Removes a group of objects from the current level by its name.
         *
         * @param groupName Name of the group to remove.
         */
        static void removeGroupByName(const std::string& groupName);

        /**
         * @brief Saves the current level's data back to disk.
         */
        static void saveCurrentLevel();

        /**
         * @brief Retrieves all loaded levels mapped by their IDs.
         *
         * @return Const reference to a map of loaded levels.
         */
        static const std::unordered_map<int, std::unique_ptr<Level>>& getLevels() { return loaded_levels_; }

        /**
         * @brief Returns the most recently loaded level.
         *
         * @return Pointer to the most recently loaded Level object, or nullptr if none loaded.
         */
        static Level* getMostRecentLoadedLevel();

    private:
        static std::vector<LevelMeta> meta_data_; ///< Cache of all level metadata loaded @note metadata files need to lie in assets/levels as .meta.json
        static std::unordered_map<int, std::unique_ptr<Level>> loaded_levels_; ///< Map of loaded levels by ID  @note Put your level json files in assets/levels
        static std::unordered_map<int, std::string> idToFilename_; ///< Mapping of level IDs to filenames.
        static Level* loadLevel(int ID, const std::string& filename); ///< Internal helper to load level from file. @note Put your level json files in assets/levels
        static int most_recent_loaded_lvl_ID; ///< ID of the last loaded level.
    };
}
