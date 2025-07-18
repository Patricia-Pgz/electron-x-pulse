#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include "engine/rendering/Texture.h"

namespace gl3::engine::rendering
{
 /**
  * @class TextureManager
  * @brief Static class for loading, caching, and managing textures in the engine.
  *
  * This manager handles texture reuse and lifetime. It supports general textures,
  * tilesets, UI textures, and background textures, organized in separate caches.
  */
 class TextureManager
 {
 public:
  /**
   * @brief Cache for regular textures.
   */
  static std::unordered_map<std::string, std::unique_ptr<Texture>> texture_cache;

  /**
   * @brief Cache for tileset textures.
   */
  static std::unordered_map<std::string, std::unique_ptr<Texture>> tile_set_cache;

  /**
   * @brief Cache for UI-specific textures.
   */
  static std::unordered_map<std::string, std::unique_ptr<Texture>> ui_texture_cache;

  /**
   * @brief Cache for background textures.
   */
  static std::unordered_map<std::string, std::unique_ptr<Texture>> bg_texture_cache;

  /**
   * @brief Add a texture to their according texture cache, defined by their parent folder name (ui, background, etc.).
   * @param key Key for accessing the texture.
   * @param path Path to the texture file.
   * @param tilesX Number of tiles horizontally (default 8).
   * @param tilesY Number of tiles vertically (default 8).
   */
  static void add(const std::string& key, const std::filesystem::path& path, int tilesX = 8, int tilesY = 8);

  /**
   * @brief Loads all textures from assets/backgroundTextures, assets/textures and assets/uiTextures.
   * @note Be careful to only put textures in these folders that you really need. they all will be loaded once on start up
   */
  static void loadTextures();

  /**
   * @brief Add all textures from a folder to their according texture cache, defined by their parent folder name (ui, background, etc.).
   * @param textureFolderPath Path to the folder.
   */
  static void addAllTexturesFromFolder(const std::filesystem::path& textureFolderPath);

  /**
   * @brief Get a texture from either the general cache or the tileset cache.
   * @param key Lookup key.
   * @return Pointer to the Texture if found, nullptr otherwise.
   */
  static const Texture* getTileOrSingleTex(const std::string& key);

  /**
   * @brief Get a UI texture by key.
   * @param key Lookup key.
   * @return Pointer to the Texture if found, nullptr otherwise.
   */
  static const Texture* getUITexture(const std::string& key);

  /**
   * @brief Get a background texture by key.
   * @param key Lookup key.
   * @return Pointer to the Texture if found, nullptr otherwise.
   */
  static const Texture* getBgTexture(const std::string& key);

  /**
   * @brief Get all general textures.
   * @return Const reference to the texture cache.
   */
  static const std::unordered_map<std::string, std::unique_ptr<Texture>>& getAllTextures()
  {
   return texture_cache;
  }

  /**
   * @brief Get all tileset textures.
   * @return Const reference to the tileset cache.
   */
  static const std::unordered_map<std::string, std::unique_ptr<Texture>>& getAllTileSets()
  {
   return tile_set_cache;
  }

  /**
   * @brief Get all UI textures.
   * @return Const reference to the UI texture cache.
   */
  static const std::unordered_map<std::string, std::unique_ptr<Texture>>& getAllUITextures()
  {
   return ui_texture_cache;
  }

  /**
   * @brief Get all background textures.
   * @return Const reference to the background texture cache.
   */
  static const std::unordered_map<std::string, std::unique_ptr<Texture>>& getAllBgTextures()
  {
   return bg_texture_cache;
  }

  /**
   * @brief Load a single texture into its corresponding cache, defined by its parent folder naming.
   * @param key Lookup key.
   * @param path Path to the texture file.
   */
  static void load(const std::string& key, const std::string& path);

  /**
   * @brief Clear all texture caches.
   */
  static void clear();
 };
}
