#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>
#include "engine/rendering/Texture.h"


namespace gl3::engine::rendering
{
    class TextureManager
    {
    public:
        static void add(const std::string& key, const std::filesystem::path& path, int tilesX = 8, int tilesY = 8);
        static void loadTextures();
        static void addAllTexturesFromFolder(const std::filesystem::path& textureFolderPath);
        static const Texture& get(const std::string& key);
        static const Texture& getUITexture(const std::string& key);

        static const std::unordered_map<std::string, Texture>& getAllTextures()
        {
            return texture_cache_;
        }

        static const std::unordered_map<std::string, Texture>& getAllTileSets()
        {
            return tile_set_cache_;
        }

        static const std::unordered_map<std::string, Texture>& getAllUITextures()
        {
            return ui_texture_cache_;
        }

        static void load(const std::string& key, const std::string& path);
        static void clear();

    private:
        static std::unordered_map<std::string, Texture> texture_cache_;
        static std::unordered_map<std::string, Texture> tile_set_cache_;
        static std::unordered_map<std::string, Texture> ui_texture_cache_;
    };
}
