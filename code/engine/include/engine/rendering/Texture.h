#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace gl3::engine::rendering
{
    /**
     * @class Texture
     * @brief Manages a 2D OpenGL texture, with optional support for tile-based UV mapping.
     *
     * This class handles loading an image file into an OpenGL texture, binding it to a texture unit,
     * and providing helper methods for tile-based UV coordinate generation.
     */
    class Texture
    {
    public:
        /**
         * @brief Construct a new Texture from an image file.
         * @param path Path to the image file.
         * @param tilesX Number of horizontal tiles if using a tileset (default 0 for none).
         * @param tilesY Number of vertical tiles if using a tileset (default 0 for none).
         */
        explicit Texture(const std::string& path, int tilesX = 0, int tilesY = 0);

        /**
         * @brief Destroy the Texture and free OpenGL resources.
         */
        ~Texture();

        /// Delete copy constructor.
        Texture(const Texture&) = delete;

        /// Delete copy assignment.
        Texture& operator=(const Texture&) = delete;

        /**
         * @brief Move constructor.
         * @param other Texture to move from.
         */
        Texture(Texture&& other) noexcept;

        /**
         * @brief Move assignment.
         * @param other Texture to move from.
         * @return Reference to this Texture.
         */
        Texture& operator=(Texture&& other) noexcept;

        /**
         * @brief Bind the texture to a given OpenGL texture unit.
         * @param slot Texture unit index (default is 0).
         */
        void bind(GLuint slot = 0) const;

        /**
         * @brief Get the OpenGL texture ID.
         * @return Texture ID.
         */
        [[nodiscard]] GLuint getID() const { return ID; }

        /**
         * @brief Get the width of the texture in pixels.
         * @return Width in pixels.
         */
        [[nodiscard]] int getWidth() const { return width; }

        /**
         * @brief Get the height of the texture in pixels.
         * @return Height in pixels.
         */
        [[nodiscard]] int getHeight() const { return height; }

        /**
         * @brief Check if the texture is a tileset.
         * @return True if it is a tileset, false otherwise.
         */
        [[nodiscard]] bool isTileSet() const { return is_tileset; }

        /**
         * @brief Get UV coordinates for a single tile in a tileset.
         * @param tileX Tile column index.
         * @param tileY Tile row index.
         * @param tilesX Number of horizontal tiles.
         * @param tilesY Number of vertical tiles.
         * @return vec4 containing (u_min, v_min, u_max, v_max).
         */
        [[nodiscard]] static glm::vec4 getTileUV(int tileX, int tileY, int tilesX, int tilesY);

        /**
         * @brief Generate UV coordinates for all tiles in a grid.
         * @param tilesX Number of horizontal tiles.
         * @param tilesY Number of vertical tiles.
         * @return Vector of vec4 UVs for each tile.
         */
        [[nodiscard]] static std::vector<glm::vec4> generateTileUVs(int tilesX = 8, int tilesY = 8);

        /**
         * @brief Get precomputed UVs for this texture if it is a tileset.
         * @return Vector of vec4 UVs.
         */
        [[nodiscard]] const std::vector<glm::vec4>& getTileUVs() const { return tile_uvs; }

    private:
        GLuint ID = 0;                          ///< OpenGL texture ID.
        int width = 0;                          ///< Width in pixels.
        int height = 0;                         ///< Height in pixels.
        bool is_tileset = false;                ///< True if texture is a tileset.
        std::vector<glm::vec4> tile_uvs;        ///< Precomputed UVs for tileset.
        std::string file_name;                  ///< Original file name.
    };
}
