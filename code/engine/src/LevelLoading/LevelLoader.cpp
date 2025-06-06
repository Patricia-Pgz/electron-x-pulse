#include "engine/levelloading/LevelLoader.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

namespace gl3::engine::levelLoading
{
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    void LevelLoader::loadAllLevels()
    {
        levels.clear();
        for (const auto& entry : fs::directory_iterator(resolveAssetPath("levels")))
        {
            if (entry.path().extension() == ".json")
            {
                try
                {
                    Level level = loadLevelFromFile(entry.path().string());
                    levels.push_back(level);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Skipping " << entry.path() << ": " << e.what() << '\n';
                }
            }
        }
    }

    Level LevelLoader::loadLevelFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Failed to open: " + filename);

        json j;
        file >> j;

        Level level;
        level.name = j["level_name"];
        level.scrollSpeed = j["scroll_speed"];
        level.playerX = j["player_start"]["x"];
        level.playerY = j["player_start"]["y"];
        level.filename = filename;

        for (const auto& obj : j["objects"])
        {
            GameObject go;
            go.type = obj["type"];
            go.x = obj["x"];
            go.y = obj["y"];
            if (obj.contains("width")) go.width = obj["width"];
            if (obj.contains("height")) go.height = obj["height"];
            if (obj.contains("color")) go.color = obj["color"];
            level.objects.push_back(go);
        }

        return level;
    }
}
