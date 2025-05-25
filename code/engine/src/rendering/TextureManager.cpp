#include "engine/rendering/TextureManager.h"

#include <regex>
#include <stdexcept>
#include <unordered_set>

#include "engine/Assets.h"

namespace gl3::engine::rendering
{
    std::unordered_map<std::string, Texture> TextureManager::texture_cache_;
    std::unordered_map<std::string, Texture> TextureManager::tile_set_cache_;
    static const std::unordered_set<std::string> validExtensions = {".png", ".jpg", ".jpeg"};

    void TextureManager::add(const std::string& key, const std::filesystem::path& path, int tilesX,
                             int tilesY)
    {
        if (!exists(path) || !is_regular_file(path) || !validExtensions.contains(path.extension().string()))
        {
            std::cerr << "Texture path is invalid: " << path << std::endl;
            return;
        }

        if (!texture_cache_.contains(key))
        {
            std::string filename = path.filename().string();
            std::ranges::transform(filename, filename.begin(), ::tolower);
            if (filename.find("tileset") != std::string::npos)
            {
                std::smatch match;
                std::regex tileSizePattern(R"(_(\d+)x(\d+))");

                if (std::regex_search(filename, match, tileSizePattern) && match.size() == 3)
                {
                    tilesX = std::stoi(match[1].str());
                    tilesY = std::stoi(match[2].str());
                }

                tile_set_cache_.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(key),
                    std::forward_as_tuple(path.string(), tilesX, tilesY)
                );
            }
            else
            {
                texture_cache_.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(key),
                    std::forward_as_tuple(path.string())
                );
            }
        }
    }

    void TextureManager::loadTextures()
    {
        const std::filesystem::path textureFolder = resolveAssetPath("textures");

        for (const auto& entry : std::filesystem::directory_iterator(textureFolder))
        {
            if (entry.is_regular_file())
            {
                std::string path = entry.path().string();
                std::string filename = entry.path().stem().string();

                std::string ext = entry.path().extension().string();
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
                {
                    add(filename, path);
                }
            }
        }
    }

    const Texture& TextureManager::get(const std::string& key)
    {
        auto it = texture_cache_.find(key);
        if (it == texture_cache_.end())
        {
            it = tile_set_cache_.find((key));
            if (it == tile_set_cache_.end())
            {
                throw std::runtime_error("TextureManager: Texture key not found: " + key);
            }
        }
        return it->second;
    }

    void TextureManager::load(const std::string& key, const std::string& path)
    {
        add(key, path);
    }

    void TextureManager::clear()
    {
        texture_cache_.clear();
    }
}
