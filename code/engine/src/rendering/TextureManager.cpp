#include "engine/rendering/TextureManager.h"
#include "engine/Assets.h"

#include <iostream>
#include <regex>
#include <stdexcept>
#include <unordered_set>

namespace gl3::engine::rendering
{
    /**
     * @brief Cache storing loaded textures.
     *
     * Maps texture names to their unique Texture instances.
     */
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::texture_cache;

    /**
     * @brief Cache for loaded tile set textures.
     */
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::tile_set_cache;

    /**
     * @brief Cache for loaded UI textures.
     */
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::ui_texture_cache;

    /**
     * @brief Cache for loaded background textures.
     */
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::bg_texture_cache;

    /**
     * @brief Valid file extensions for texture loading.
     *
     * Only files with these extensions are considered valid textures.
     */
    static const std::unordered_set<std::string> validExtensions = {".png", ".jpg", ".jpeg"};

    void TextureManager::add(const std::string& key, const std::filesystem::path& path, int tilesX, int tilesY)
    {
        if (!exists(path) || !is_regular_file(path) || !validExtensions.contains(path.extension().string()))
        {
            std::cerr << "Texture path is invalid: " << path << std::endl;
            return;
        }

        if (texture_cache.contains(key) || tile_set_cache.contains(key) ||
            ui_texture_cache.contains(key) || bg_texture_cache.contains(key))
        {
            return;
        }

        std::string filename = path.filename().string();
        std::ranges::transform(filename, filename.begin(), ::tolower);

        auto parent = path.parent_path().filename().string();
        std::ranges::transform(parent, parent.begin(), ::tolower);

        if (parent.find("ui") != std::string::npos)
        {
            ui_texture_cache.emplace(key, std::make_unique<Texture>(path.string()));
            return;
        }

        if (parent.find("background") != std::string::npos)
        {
            bg_texture_cache.emplace(key, std::make_unique<Texture>(path.string()));
            return;
        }

        if (filename.find("tileset") != std::string::npos)
        {
            std::smatch match;

            if (const std::regex tileSizePattern(R"(_(\d+)x(\d+))"); std::regex_search(filename, match, tileSizePattern)
                && match.size() == 3)
            {
                tilesX = std::stoi(match[1].str());
                tilesY = std::stoi(match[2].str());
            }

            tile_set_cache.emplace(key, std::make_unique<Texture>(path.string(), tilesX, tilesY));
        }
        else
        {
            texture_cache.emplace(key, std::make_unique<Texture>(path.string()));
        }
    }


    void TextureManager::loadTextures()
    {
        const std::filesystem::path textureFolder = resolveAssetPath("textures");
        addAllTexturesFromFolder(textureFolder);
        const std::filesystem::path uiTextureFolder = resolveAssetPath("uiTextures");
        addAllTexturesFromFolder(uiTextureFolder);
        const std::filesystem::path bgTextureFolder = resolveAssetPath("backgroundTextures");
        addAllTexturesFromFolder(bgTextureFolder);
    }

    void TextureManager::addAllTexturesFromFolder(const std::filesystem::path& textureFolderPath)
    {
        for (const auto& entry : std::filesystem::directory_iterator(textureFolderPath))
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

    const Texture* TextureManager::getTileOrSingleTex(const std::string& key)
    {
        auto tex = texture_cache.find(key);
        if (tex == texture_cache.end())
        {
            tex = tile_set_cache.find((key));
            if (tex == tile_set_cache.end())
            {
                tex = bg_texture_cache.find((key));
                if (tex == bg_texture_cache.end())
                {
                    throw std::runtime_error("TextureManager: Texture key not found: " + key);
                }
            }
        }
        return tex->second.get();
    }

    const Texture* TextureManager::getUITexture(const std::string& key)
    {
        const auto tex = ui_texture_cache.find(key);
        if (tex == ui_texture_cache.end())
        {
            throw std::runtime_error("TextureManager: UI-Texture key not found: " + key);
        }
        return tex->second.get();
    }

    const Texture* TextureManager::getBgTexture(const std::string& key)
    {
        const auto tex = bg_texture_cache.find(key);
        if (tex == bg_texture_cache.end())
        {
            throw std::runtime_error("TextureManager: Bg-Texture key not found: " + key);
        }
        return tex->second.get();
    }


    void TextureManager::load(const std::string& key, const std::string& path)
    {
        add(key, path);
    }

    void TextureManager::clear()
    {
        texture_cache.clear();
        tile_set_cache.clear();
        ui_texture_cache.clear();
        bg_texture_cache.clear();
    }
}
