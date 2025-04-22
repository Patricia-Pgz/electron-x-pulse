#pragma once
#include <unordered_map>
#include <string>
#include "Texture.h"

namespace gl3::engine::rendering
{
    class TextureManager
    {
    public:
        // Get or load texture
        static const Texture& get(const std::string& path);

        static void preload(const std::string& path);
        static void clear();

    private:
        static std::unordered_map<std::string, Texture> textureCache;
    };
}
