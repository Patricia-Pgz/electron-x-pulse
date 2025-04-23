#pragma once
#include <string>
#include <unordered_map>
#include "engine/rendering/Texture.h"

namespace gl3::engine::rendering
{
    class TextureManager
    {
    public:
        static void add(const std::string& key, const std::string& path);
        static void loadTextures();
        static const Texture& get(const std::string& key);
        static void load(const std::string& key, const std::string& path);
        static void clear();

    private:
        static std::unordered_map<std::string, Texture> textureCache;
    };
}
