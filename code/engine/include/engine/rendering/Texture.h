#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace gl3::engine::rendering
{
    class Texture
    {
    public:
        explicit Texture(const std::string& path, int tilesX = 0, int tilesY = 0);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        void bind(GLuint slot = 0) const;

        [[nodiscard]] GLuint getID() const { return ID; }
        [[nodiscard]] int getWidth() const { return width; }
        [[nodiscard]] int getHeight() const { return height; }
        [[nodiscard]] bool isTileSet() const { return is_tileset_; }

        // For tile-based UVs
        [[nodiscard]] static glm::vec4 getTileUV(int tileX, int tileY, int tilesX, int tilesY);
        [[nodiscard]] static std::vector<glm::vec4> generateTileUVs(int tilesX = 8, int tilesY = 8);
        [[nodiscard]] const std::vector<glm::vec4>& getTileUVs() const { return tile_uvs_; }

    private:
        GLuint ID = 0;
        int width = 0;
        int height = 0;
        bool is_tileset_ = false;
        std::vector<glm::vec4> tile_uvs_;
        std::string file_name;
    };
}
