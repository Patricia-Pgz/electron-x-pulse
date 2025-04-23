#include "engine/rendering/TextureManager.h"

#include <stdexcept>

namespace gl3::engine::rendering
{
    std::unordered_map<std::string, Texture> TextureManager::textureCache;

    void TextureManager::add(const std::string& key, const std::string& path)
    {
        if (!textureCache.contains(key))
        {
            textureCache.emplace(key, Texture(path));
        }
    }

    void TextureManager::loadTextures() {
        add("player", "textures/geometry-dash.png");
        add("platform", "textures/Tile_03.png");
        //add("ground","..."); TODO
        //add("background","...");
        //add("obstacle", "textures/obstacle.png");
    }

    const Texture& TextureManager::get(const std::string& key)
    {
        const auto it = textureCache.find(key);
        if (it == textureCache.end())
        {
            throw std::runtime_error("TextureManager: Texture key not found: " + key);
        }
        return it->second;
    }

    void TextureManager::load(const std::string& key, const std::string& path)
    {
        add(key, path);
    }

    void TextureManager::clear()
    {
        textureCache.clear();
    }
}
