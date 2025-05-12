#include "engine/rendering/TextureManager.h"

#include <filesystem>
#include <stdexcept>

namespace gl3::engine::rendering
{
    std::unordered_map<std::string, Texture> TextureManager::texture_cache_;
    std::unordered_map<std::string, Texture> TextureManager::tile_set_cache_;

    void TextureManager::add(const std::string& key, const std::string& path, const int tilesX, const int tilesY)
    {
        if (!texture_cache_.contains(key))
        {
            std::string filename = std::filesystem::path(path).filename().string();
            std::ranges::transform(filename, filename.begin(), ::tolower);
            if (filename.find("tileset") != std::string::npos)
            {
                tile_set_cache_.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(key),
                    std::forward_as_tuple(path,tilesX,tilesY)
                );
            }
            else
            {
                texture_cache_.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(key),
                    std::forward_as_tuple(path)
                );
            }
        }
    }

    void TextureManager::loadTextures()
    {
        //TODO das ist Spiel spezifisch -> auslagern?
        add("Player", "textures/geometry-dash.png");
        add("Platform", "textures/Tile_03.png");
        add("TileSet", "textures/Tileset.png", 9, 9);
        //add("worldTileSet", "textures/world_tileset.png", 16, 16);
        //add("xmasTileSet", "textures/xmas_game_tileset.png", 64, 64);
        //add("ground","..."); TODO
        //add("background","...");
        //add("obstacle", "textures/obstacle.png");
    }

    const Texture& TextureManager::get(const std::string& key)
    {
        auto it = texture_cache_.find(key);
        if (it == texture_cache_.end())
        {
            it = tile_set_cache_.find((key));
            if(it == tile_set_cache_.end())
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
