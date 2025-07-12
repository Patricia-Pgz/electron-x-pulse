#include "engine/rendering/TextureManager.h"

#include <regex>
#include <stdexcept>
#include <unordered_set>

#include "engine/Assets.h"

namespace gl3::engine::rendering
{
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::texture_cache_;
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::tile_set_cache_;
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::ui_texture_cache_;
    std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::bg_texture_cache_;
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
            if (path.parent_path().filename().string().find("ui") != std::string::npos)
            {
                ui_texture_cache_.emplace(
                    key,
                    std::make_unique<Texture>(path.string())
                );
                return;
            }
            if (path.parent_path().filename().string().find("background") != std::string::npos)
            {
                bg_texture_cache_.emplace(
                    key,
                    std::make_unique<Texture>(path.string())
                );
                return;
            }
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
                    key,
                    std::make_unique<Texture>(path.string(), tilesX, tilesY)
                );
            }
            else
            {
                texture_cache_.emplace(
                    key,
                    std::make_unique<Texture>(path.string())
                );
            }
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
        auto tex = texture_cache_.find(key);
        if (tex == texture_cache_.end())
        {
            tex = tile_set_cache_.find((key));
            if (tex == tile_set_cache_.end())
            {
                tex = bg_texture_cache_.find((key));
                if (tex == bg_texture_cache_.end())
                {
                    throw std::runtime_error("TextureManager: Texture key not found: " + key);
                }
            }
        }
        return tex->second.get();
    }

    const Texture* TextureManager::getUITexture(const std::string& key)
    {
        const auto tex = ui_texture_cache_.find(key);
        if (tex == ui_texture_cache_.end())
        {
            throw std::runtime_error("TextureManager: UI-Texture key not found: " + key);
        }
        return tex->second.get();
    }

    const Texture* TextureManager::getBgTexture(const std::string& key)
    {
        const auto tex = bg_texture_cache_.find(key);
        if (tex == bg_texture_cache_.end())
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
        texture_cache_.clear();
    }
}
