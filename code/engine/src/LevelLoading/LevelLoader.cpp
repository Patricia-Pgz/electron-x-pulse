#include "engine/levelloading/LevelLoader.h"
#include <filesystem>
#include <glaze/json/read.hpp>

namespace gl3::engine::levelLoading
{
    namespace fs = std::filesystem;

    Level LevelLoader::loadLevel(const std::filesystem::path& path)
    {
        std::ifstream file(path);
        if (!file)
        {
            throw std::runtime_error("Failed to open level file: " + path.string());
        }

        std::string json((std::istreambuf_iterator(file)),
                         std::istreambuf_iterator<char>());

        Level level;
        glz::read_json(level, json);
        return level;
    }

    std::vector<LevelMeta> LevelLoader::loadAllMetaData(
        const std::filesystem::path& levelDir)
    {
        std::vector<LevelMeta> metaData;

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
                    throw std::cerr << err.custom_error_message;
                }

                meta.fileName = path.filename().string();
                metaData.emplace_back(std::move(meta));
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error reading " << path << ": " << e.what() << '\n';
            }
        }

        return metaData;
    }
}
