#include "engine/rendering/TextureManager.h"

namespace gl3::engine::rendering
{
    std::unordered_map<std::string, Texture> TextureManager::textureCache;

    const Texture& TextureManager::get(const std::string& path)
    {
        auto it = textureCache.find(path);
        if (it == textureCache.end())
        {
            it = textureCache.emplace(path, Texture(path)).first;
        }
        return it->second;
    }

    void TextureManager::preload(const std::string& path)
    {
        get(path);
    }

    void TextureManager::clear()
    {
        textureCache.clear();
    }
}
